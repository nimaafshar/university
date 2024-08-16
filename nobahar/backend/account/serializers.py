from rest_framework import serializers
from .models import Group, User, JoinRequest, ConnectionRequest
import datetime
import time
from django.utils import timezone


class GroupReadCompactSerializer(serializers.ModelSerializer):
    class Meta:
        model = Group
        fields = ('id', 'name', 'description')
        read_only_fields = ('id', 'name', 'description')


class GroupCreateSerializer(serializers.ModelSerializer):
    id = serializers.CharField(read_only=True)

    class Meta:
        model = Group
        fields = ('id', 'name', 'description')
        read_only_fields = ('id',)
        extra_kwargs = {
            'name': {'write_only': True},
            'description': {'write_only': True},
        }

    def create(self, validated_data):
        return Group.objects.create(**validated_data, admin=self.context['request'].user)


class UserInGroupReadSerializer(serializers.ModelSerializer):
    rule = serializers.SerializerMethodField(method_name='is_owner')

    class Meta:
        model = User
        fields = ('id', 'name', 'email', 'rule')

    def is_owner(self, instance):
        if self.context['owner'].id == instance.id:
            return "Owner"
        else:
            return "normal"


class GroupReadDetailedSerializer(serializers.ModelSerializer):
    members = serializers.SerializerMethodField(method_name='get_members')

    class Meta:
        model = Group
        fields = ('name', 'description', 'members')
        read_only_fields = ('name', 'description', 'members')

    def get_members(self, instance):
        serializer = UserInGroupReadSerializer(instance.members, context={'owner': instance.admin}, many=True)
        return serializer.data


class RegisterSerializer(serializers.ModelSerializer):
    class Meta:
        model = User
        fields = ('id', 'name', 'email', 'password')
        extra_kwargs = {
            'password': {'write_only': True},
        }

    def create(self, validated_data):
        user = User(username=validated_data['email'], name=validated_data['name'],
                    email=validated_data['email'])
        user.set_password(validated_data['password'])
        user.save()
        return user


class UserSerializer(serializers.ModelSerializer):
    class Meta:
        model = User
        fields = '__all__'


class TimestampField(serializers.Field):
    def to_representation(self, value):
        return int(time.mktime(value.timetuple()))


class JoinRequestReadSerializer(serializers.ModelSerializer):
    groupId = serializers.IntegerField(source='group_id', read_only=True)
    userId = serializers.IntegerField(source='user_id', read_only=True)
    date = TimestampField()

    class Meta:
        model = JoinRequest
        fields = ('id', 'groupId', 'userId', 'date')


class JoinRequestCreateSerializer(serializers.ModelSerializer):
    groupId = serializers.PrimaryKeyRelatedField(queryset=Group.objects.all(), source='group')

    class Meta:
        model = JoinRequest
        fields = ('groupId',)

    def create(self, validated_data):
        return JoinRequest.objects.create(**validated_data, user=self.context['user'])


class AcceptRequestSerializer(serializers.Serializer):
    joinRequestId = serializers.PrimaryKeyRelatedField(queryset=JoinRequest.objects.all())

    class Meta:
        fields = ('joinRequestId',)

    def validate(self, attrs):
        validated_data = super().validate(attrs)
        user = self.context['user']
        join_request = validated_data['joinRequestId']
        if not join_request.user.has_group and join_request.group.admin == user:
            return validated_data
        else:
            raise serializers.ValidationError("not enough permissions")

    def create(self, validated_data):
        join_request = validated_data['joinRequestId']
        user = join_request.user
        user.group = join_request.group
        user.save()
        return user


class ConnectionRequestCreateSerializer(serializers.ModelSerializer):
    groupId = serializers.PrimaryKeyRelatedField(queryset=Group.objects.all(), source='to_group')

    class Meta:
        model = ConnectionRequest
        fields = ('groupId',)
        extra_kwargs = {
            'groupId': {'write_only': True},
        }

    def validate(self, attrs):
        validated_data = super().validate(attrs)
        to_group = validated_data['to_group']
        from_group = self.context['user'].group
        if from_group == to_group:
            raise serializers.ValidationError()
        else:
            return validated_data

    def create(self, validated_data):
        return ConnectionRequest.objects.create(**validated_data, from_group=self.context['user'].group)


class ConnectionRequestReadSerializer(serializers.ModelSerializer):
    connectionRequestId = serializers.IntegerField(source='id', read_only=True)
    groupId = serializers.IntegerField(source='to_group_id', read_only=True)
    sent = TimestampField()

    class Meta:
        model = ConnectionRequest
        fields = ('connectionRequestId', 'groupId', 'sent')
        read_only_fields = ('connectionRequestId', 'groupId', 'sent')


class AcceptConnectionRequestSerializer(serializers.Serializer):
    groupId = serializers.PrimaryKeyRelatedField(queryset=Group.objects.all())

    class Meta:
        fields = ('groupId',)

    def validate(self, attrs):
        validated_data = super().validate(attrs)
        user = self.context['user']
        to_group = user.group
        from_group = validated_data['groupId']
        try:
            ConnectionRequest.objects.get(to_group=to_group, from_group=from_group)
        except ConnectionRequest.DoesNotExist:
            raise serializers.ValidationError("does not exist")

        if to_group.connected_to.filter(id=from_group.id).exists():
            raise serializers.ValidationError("already exist")

        return validated_data

    def create(self, validated_data):
        user = self.context['user']
        to_group = user.group
        from_group = validated_data['groupId']
        to_group.connected_to.add(from_group)
        from_group.connected_to.add(to_group)
        return from_group

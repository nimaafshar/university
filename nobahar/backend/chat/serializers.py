
from rest_framework import serializers
from .models import *


class ChatSerializer(serializers.Serializer):
    userId = serializers.IntegerField
    name = serializers.CharField


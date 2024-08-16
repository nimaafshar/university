from rest_framework_simplejwt.serializers import TokenObtainSerializer, RefreshToken, TokenObtainPairSerializer
from rest_framework_simplejwt.views import TokenObtainPairView
from account.models import User
from rest_framework.exceptions import AuthenticationFailed
from .error_handlers import BadRequest


class EmailTokenObtainSerializer(TokenObtainSerializer):
    username_field = User.EMAIL_FIELD


class CustomTokenObtainPairSerializer(EmailTokenObtainSerializer):
    @classmethod
    def get_token(cls, user):
        return RefreshToken.for_user(user)

    def validate(self, attrs):
        try:
            data = super().validate(attrs)
        except AuthenticationFailed:
            raise BadRequest()

        refresh = self.get_token(self.user)

        data["token"] = str(refresh.access_token)
        data["message"] = "successfull"

        return data


class EmailTokenObtainPairView(TokenObtainPairView):
    serializer_class = CustomTokenObtainPairSerializer

from rest_framework import generics
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response

from account.models import User
from .serializers import *
from .models import *
from django.db.models import Q
from account.error_handlers import BadRequest


class ChatsList(generics.GenericAPIView):
    serializer_class = ChatSerializer
    permission_classes = [IsAuthenticated]

    def get(self, request, *args, **kwargs):
        user = request.user
        messages = Message.objects.filter(Q(sent_by=user) | Q(sent_to=user)).order_by('date')
        users_id = []
        chats = {"chats": []}
        for message in messages:

            if message.sent_to.id == user.id:
                if message.sent_by.id not in users_id:
                    chats['chats'].insert(0, {"userId": message.sent_by.id, "name": message.sent_by.name})
                    users_id.append(message.sent_by.id)
            else:
                if message.sent_to.id not in users_id:
                    chats['chats'].insert(0, {"userId": message.sent_to.id, "name": message.sent_to.name})
                    users_id.append(message.sent_to.id)

        return Response(chats)


class Chat(generics.GenericAPIView):
    serializer_class = ChatSerializer
    permission_classes = [IsAuthenticated]

    def get(self, request, user_id, *args, **kwargs):
        print(user_id)
        otherid = user_id
        user = request.user
        other = User.objects.get(id=otherid)
        messages = Message.objects.filter(Q(sent_by=user, sent_to=other) | Q(sent_by=other, sent_to=user)).order_by(
            'date')
        res = {"messages": []}
        for message in messages:
            res['messages'].insert(0, {"message": message.message, "date": message.date, "sentby": message.sent_by.id})

        return Response(res)

    def post(self, request, user_id, *args, **kwargs):
        print("salam")
        other_id = user_id
        user = request.user
        print(user)
        other = User.objects.get(id=other_id)
        print(other)
        if not user.can_chat_to(other):
            print("1")
            raise BadRequest()

        message_text = request.data.get('message')
        if not message_text:
            print("2")
            raise BadRequest()

        Message.objects.create(message=message_text, sent_by=user, sent_to=other)
        return Response({"message": "successfull"})

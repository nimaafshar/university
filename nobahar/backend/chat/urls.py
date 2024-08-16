from django.urls import path
from .views import *

urlpatterns = [
                  path('chats/', ChatsList.as_view()),
                  path('chats/<int:user_id>/', Chat.as_view())
              ]
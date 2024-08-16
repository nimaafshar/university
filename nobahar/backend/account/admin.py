from django.contrib import admin
from .models import JoinRequest, ConnectionRequest, Group, User

# Register your models here.
admin.site.register(User)
admin.site.register(Group)
admin.site.register(JoinRequest)
admin.site.register(ConnectionRequest)

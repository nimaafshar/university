from rest_framework.permissions import BasePermission


class UserGroupPermissions(BasePermission):

    def has_permission(self, request, view):
        if view.action == 'create':
            return not request.user.has_group
        elif view.action == 'my_group':
            return request.user.has_group
        else:
            return True


class UserJoinRequestPermissions(BasePermission):
    def has_permission(self, request, view):
        if view.action == 'group_requests':
            return request.user.is_admin
        elif view.action == 'create':
            return not request.user.has_group
        elif view.action == 'accept_request':
            return request.user.is_admin
        else:
            return True


class UserConnectionRequestPermissions(BasePermission):
    def has_permission(self, request, view):
        if view.action == 'create' or view.action == 'list' or view.action == 'accept_request':
            return request.user.is_admin
        else:
            return True

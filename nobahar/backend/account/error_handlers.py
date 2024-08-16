from django.http import JsonResponse
from rest_framework import status
from rest_framework.exceptions import APIException
from rest_framework.views import exception_handler

DEFAULT_BAD_REQUEST_RESPONSE = {
    "error": {
        "enMessage": "Bad request!"
    }
}


def custom_exception_handler(exc, context):
    response = exception_handler(exc, context)

    if response and (response.status_code == 400 or response.status_code == 403 or response.status_code == 401):
        response.data = DEFAULT_BAD_REQUEST_RESPONSE
        response.status_code = 400
    return response


class BadRequest(APIException):
    status_code = 400


def bad_request(request, exception, *args, **kwargs):
    """
    My Generic 400 error handler.
    """
    data = {
        "error": {
            "enMessage": "Bad request!"
        }
    }
    return JsonResponse(data, status=status.HTTP_400_BAD_REQUEST)

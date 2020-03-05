from django.urls import re_path
from . import consumers

websocket_urlpatterns = [
    re_path(r'ws/management/(?P<management>\w+)/$', consumers.ChatConsumer),
]
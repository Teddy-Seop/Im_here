from django.urls import re_path
from . import consumers

websocket_urlpatterns = [
    re_path(r'ws/menu/(?P<menu>\w+)/$', consumers.ChatConsumer),
]
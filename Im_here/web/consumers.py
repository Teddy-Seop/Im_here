from channels.generic.websocket import AsyncWebsocketConsumer
import json

class ChatConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        self.management = self.scope['url_route']['kwargs']['management']
        self.management_group = 'chat_%s' % self.management

        # Join room group
        await self.channel_layer.group_add(
            self.management_group,
            self.channel_name
        )

        await self.accept()

    async def disconnect(self, close_code):
        # Leave room group
        await self.channel_layer.group_discard(
            self.management_group,
            self.channel_name
        )

    # Receive message from WebSocket
    async def receive(self, text_data):
        text_data_json = json.loads(text_data)
        message = text_data_json['message']
        type = text_data_json['type']

        # Send message to room group
        await self.channel_layer.group_send(
            self.management_group,
            {
                'type': 'chat_message',
                'message': message,
                'type': type
            }
        )

    # Receive message from room group
    async def chat_message(self, event):
        message = event['message']
        type = event['type']

        # Send message to WebSocket
        await self.send(text_data=json.dumps({
            'message': message,
            'type': type
        }))
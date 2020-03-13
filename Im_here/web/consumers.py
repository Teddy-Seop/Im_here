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
        device_num = text_data_json['device_num']
        option = text_data_json['option']
        message = text_data_json['message']
        if option == 'GPS':
            lat = text_data_json['lat']
            lan = text_data_json['lan']
            # Send message to room group
            await self.channel_layer.group_send(
                self.management_group,
                {
                    'type': 'chat_message',
                    'device_num': device_num,
                    'option': option,
                    'message': message,
                    'lat': lat,
                    'lan': lan
                }
            )
        else:
            # Send message to room group
            await self.channel_layer.group_send(
                self.management_group,
                {
                    'type': 'chat_message',
                    'device_num': device_num,
                    'option': option,
                    'message': message
                }
            )

    # Receive message from room group
    async def chat_message(self, event):
        device_num = event['device_num']
        print(device_num)
        option = event['option']
        message = event['message']
        if option == 'GPS':
            lat = event['lat']
            lan = event['lan']
            # Send message to WebSocket
            await self.send(text_data=json.dumps({
                'device_num': device_num,
                'option': option,
                'message': message,
                'lat': lat,
                'lan': lan
            }))
        else:
            # Send message to WebSocket
            await self.send(text_data=json.dumps({
                'device_num': device_num,
                'option': option,
                'message': message
            }))
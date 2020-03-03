from django.shortcuts import render
from django.utils.safestring import mark_safe
import json

def index(request):
    
    # 임의 좌표값
    location = [
        {
            "dno": 1,
            "lat": 37.6451218,
            "lan": 127.106431
        },
        {
            "dno": 2,
            "lat": 37.6470070,
            "lan": 127.113799
        },
        {
            "dno": 3,
            "lat": 37.6464013,
            "lan": 127.115940
        }
    ]

    return render(request, 'web/index.html', context={'location': location})

def menu(request, menu):
    return render(request, 'web/menu.html', {
        'menu': mark_safe(json.dumps(menu))
    })

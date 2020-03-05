from django.shortcuts import render
from django.utils.safestring import mark_safe
import json

def index(request):
    return render(request, 'web/index.html', {})

def management(request, management):
    
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

    return render(request, 'web/management.html', {
        'management': mark_safe(json.dumps(management)),
        'location': location
    })

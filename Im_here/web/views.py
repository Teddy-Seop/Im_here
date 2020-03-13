from django.shortcuts import render
from django.utils.safestring import mark_safe
from django.views.decorators.csrf import csrf_exempt
from django.http import HttpResponse, JsonResponse
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
        # {
        #     "dno": 2,
        #     "lat": 37.6470070,
        #     "lan": 127.113799
        # },
        # {
        #     "dno": 3,
        #     "lat": 37.6464013,
        #     "lan": 127.115940
        # }
    ]

    return render(request, 'web/management.html', {
        'management': mark_safe(json.dumps(management)),
        'location': location
    })

@csrf_exempt
def map(request, management):
    dno = request.POST.get('device_num')
    lat = request.POST.get('lat')
    lan = request.POST.get('lan')
    print(dno)
    location = [
        {
            "dno": dno,
            "lat": lat,
            "lan": lan
        }
    ]

    return HttpResponse(json.dumps(location), content_type="application/json")

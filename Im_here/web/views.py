from django.shortcuts import render
from django.utils.safestring import mark_safe
import json

def index(request):
    return render(request, 'web/index.html', {})

def menu(request, menu):
    return render(request, 'web/menu.html', {
        'menu': mark_safe(json.dumps(menu))
    })

import urllib.request, json, sys

lat, lon, data, ora, prognoza = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), sys.argv[5]
 
# Extractia temperaturii din API Open-Meteo
base = "https://api.open-meteo.com/v1/forecast" if prognoza == "1" \
     else "https://archive-api.open-meteo.com/v1/archive"
 
url = f"{base}?latitude={lat}&longitude={lon}&start_date={data}&end_date={data}&hourly=temperature_2m&timezone=Europe%2FBucharest"
 
data = json.loads(urllib.request.urlopen(url).read())
print(data["hourly"]["temperature_2m"][ora])
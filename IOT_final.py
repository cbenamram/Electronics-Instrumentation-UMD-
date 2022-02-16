import os, glob, time, json
import paho.mqtt.client as mqtt


from RPLCD import CharLCD
lcd = CharLCD(cols=16, rows=2, pin_rs=37, pin_e=35, pins_data=[33, 31, 29, 23])

THINGSBOARD_HOST = '10.105.212.133'
ACCESS_TOKEN = '7OelxpkRvvnxNoqU6Wvb'

os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

base_dir = '/sys/bus/w1/devices/'
device_folder = glob.glob(base_dir + '28*')[0]
device_file = device_folder + '/w1_slave'

def on_connect(client, userdata, rc, *extra_params):
  print('Connected with result code ' + str(rc))
  client.subscribe('v1/devices/me/rpc/request/+')  
  payload = json.dumps({'Automated':0, 'Manual':0}) #both temp swtiches starting off
  client.publish('v1/devices/me/attributes', payload, 1) #publishing our payload dictionary
  
def on_message(client, userdata, msg):   
    global ManualFanSpeed #inialized Tset=0 but need it to change so must declare as global variable
    data = json.loads(msg.payload)
    print(data['method'], data['params'])
    
    if data['method'] == 'setFanSpeed':      # Temperature dial control
        ManualFanSpeed = float(data['params'])  #extracting the value associated with params
        
    if ManualFanSpeed not in range(AutomatedFanSpeed - 3, AutomatedFanSpeed + 3):
        #gpio output manual fan speed
    else:
        #gpio output automated fan speed from temperature mapped values
        
def map(x, MinOperatingTemp, MaxOperatingTemp, MinFanSpeed,MaxFanSpeed):
    AutomatedFanSpeed = (x-MinOperatingTemp)*(MaxFanSpeed-MinFanSpeed)/(MaxOperatingTemp-MinOperatingTemp) + MinFanSpeed
    return AutomatedFanSpeed 

def SensorReading():
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines

def TemperatureOutput():
    lines = SensorReading()
    while lines[0].strip()[-3:] != 'YES': #checking first line contains YES indicates line with temp value
        time.sleep(0.2)
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=') #finding the line that contains t =; lines[1] seeks 2nd element/line in the array
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:] #start position of temp t, adding 2 to obtain temperature value
        temp_c = float(temp_string) / 1000.0
        TemperatureValue = temp_c * 9.0 / 5.0 + 32.0
        return temp_c, TemperatureValue
        print(TemperatureValue)
        
def LCD_Output():
    lcd.cursor_pos = (0, 0)
    lcd.write_string("Temp: " + TemperatureValue() + unichr(223) + "F")
    lcd.cursor_pos = (1,0)
    lcd.write_string("Speed Capacity: " + TemperatureValue() + unichr(223) + "%")    


client = mqtt.Client()                # Create a Client object
client.on_connect = on_connect        # Register connect callback
client.on_message = on_message        # Registed publish message callback
client.username_pw_set(ACCESS_TOKEN)  # Set access token

connect_ok = False
while not connect_ok:  
  try:
    client.connect(THINGSBOARD_HOST, 1883, 60) 
    connect_ok = True
  except:
    print ('Thingsboard server not responding...may be starting up')
    time.sleep(10)    
client.loop_start()


try:
    data = {'TemperatureValue':0, 'FanSpeed':0}   # start with empty dictionary for time series data
    while True:
        TemperatureOutput()
        LCD_Output()
        data['TemperatureValue'] = TemperatureValue
        data['FanSpeed'] = FanSpeed
        client.publish('v1/devices/me/telemetry', json.dumps(data), 1)
        time.sleep(1)

except KeyboardInterrupt:
  pass

client.loop_stop()
client.disconnect()
    
    
    
    
    

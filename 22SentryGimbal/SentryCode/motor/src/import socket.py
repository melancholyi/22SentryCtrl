import socket
new_socket = socket.socket()
ip = "192.168.43.159"
print(ip)
port = 8080
print(port)
new_socket.bind((ip,port))
print("init")
new_socket.listen(100)
conn,addr = new_socket.accept()
print('µÿ÷∑addr:',addr)



import ujson
devices={}
devices["name"]="saaa"
devices["EnvTemp"]=20
devices["EnvHumidity"]=10
devices["AtomosPressure"]=999
devices["GasDensity"]=555
devices["DeviceName"]=3
devices["DeviceId"]=5
devices["RoundSpeed"]=7
devices["WorkTime"]=8
devices["CurrentOutput"]=9
devices["WorkTemp"]=555
devices["xpos"]=0
devices["ypos"]=0
devices["zpos"]=0

devices5={}
devices5["name"]="bbbbbbbbb"
devices5["EnvTemp"]=20
devices5["EnvHumidity"]=10
devices5["AtomosPressure"]=999
devices5["GasDensity"]=555
devices5["DeviceName"]=3
devices5["DeviceId"]=5
devices5["RoundSpeed"]=7
devices5["WorkTime"]=8
devices5["CurrentOutput"]=9
devices5["WorkTemp"]=555
devices5["xpos"]=0
devices5["ypos"]=0
devices5["zpos"]=0


ll = []
ll.append(devices)
ll.append(devices5)
jsonString={}
jsonString["devices"]=ll


x=ujson.dumps(jsonString)
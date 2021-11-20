import time
from . import DeviceClasses

ECU_STATES = [
    "IDLE",
    "SETUP",
    "RUN"
]

BMS_WARNINGS = [
    "WARN_CELL_LOW_VOLTAGE",
    "WARN_CELL_DROPPING",
    "WARN_PRECHARGE_FAIL"
]

BMS_ERRORS = [
    "ERROR_LTC6804_PEC_ERROR",
    "ERROR_CELL_UNDER_VOLTAGE",
    "ERROR_CELL_OVER_VOLTAGE",
    "ERROR_CELL_OVER_TEMPERATURE",
    "ERROR_OVER_CURRENT",
    "ERROR_CAN"
]

class Parser:
    def __init__(self):

        self.a = DeviceClasses.Accel_Gyro()
        self.g = DeviceClasses.Accel_Gyro()
        self.bmsLV = DeviceClasses.BMS()
        self.bmsHV = DeviceClasses.BMS()
        self.bmsHV_state = DeviceClasses.BMS_state()
        self.steer = DeviceClasses.Steer()
        self.pedals = DeviceClasses.Pedals()
        self.speed = DeviceClasses.Speed()
        self.invl = DeviceClasses.Inverter()
        self.invr = DeviceClasses.Inverter()
        self.l_enc = DeviceClasses.Encoder()
        self.r_enc = DeviceClasses.Encoder()
        self.ecu = DeviceClasses.ECU()
        self.ecu_state = DeviceClasses.ECU_state()
        self.steeringWheel = DeviceClasses.SteeringWheel()
        self.cmds = DeviceClasses.Commands()
        self.gps = DeviceClasses.GPS()
        #self.a2 = DeviceClasses.Accel_Gyro()
        #self.g2 = DeviceClasses.Accel_Gyro()

        self.sensors = []

        self.cmds.time = time.time()

        self.a.type = "Accel"
        self.g.type = "Gyro"
        self.bmsLV.type = "BMS LV"
        self.bmsHV.type = "BMS HV"
        self.steer.type = "Steer"
        self.pedals.type = "Pedals"
        self.l_enc.type = "Encoder Left"
        self.r_enc.type = "Encoder Right"
        self.invl.type = "Inverter Left"
        self.invr.type = "Inverter Right"
        self.ecu.type = "ECU"
        self.ecu_state.type = "State ECU"
        self.bmsHV_state.type = "State BMS HV"
        self.steeringWheel.type = "SteeringWheel"
        #self.a2.type = "Accel IZZE"
        #self.g2.type = "Gyro IZZE"

        self.sensors.append(self.ecu)
        self.sensors.append(self.steeringWheel)
        self.sensors.append(self.cmds)
        self.sensors.append(self.a)
        self.sensors.append(self.g)
        #self.sensors.append(self.a2)
        #self.sensors.append(self.g2)
        self.sensors.append(self.speed)
        self.sensors.append(self.steer)
        self.sensors.append(self.pedals)
        self.sensors.append(self.invl)
        self.sensors.append(self.invr)
        self.sensors.append(self.bmsLV)
        self.sensors.append(self.bmsHV)
        self.sensors.append(self.gps)

    def parseMessage(self, timestamp, id, msg):
        modifiedSensors = []

        if(id == 0xB0):
            # PEDALS
            if(msg[0] == 0x01):
                self.pedals.throttle1 = msg[1]
                self.pedals.throttle2 = msg[2]
                self.pedals.time = timestamp

                modifiedSensors.append(self.pedals)
                #modifiedSensors.append(self.pedals.type)
            if(msg[0] == 0x02):
                self.pedals.brake = msg[1]
                self.pedals.front = (msg[2] * 256 + msg[4]) / 500
                self.pedals.back = (msg[5] * 256 + msg[7]) / 500
                self.pedals.time = timestamp

                modifiedSensors.append(self.pedals)
                #modifiedSensors.append(self.pedals.type)

        if(id == 0x4ED):
            # ACC
            self.a.scale = 8
            self.a.x = (msg[0] * 256 + msg[1])
            self.a.y = (msg[2] * 256 + msg[3])
            self.a.z = (msg[4] * 256 + msg[5])

            if(self.a.x > 32768):
                self.a.x -= 65536
            if(self.a.y > 32768):
                self.a.y -= 65536
            if(self.a.z > 32768):
                self.a.z -= 65536

            self.a.x /= 100
            self.a.y /= 100
            self.a.z /= 100

            self.a.time = timestamp

            modifiedSensors.append(self.a)
            #modifiedSensors.append(self.a.type)

        if(id == 0x4EC):
            # GYRO
            self.g.scale = 245
            self.g.x = (msg[0] * 256 + msg[1])
            self.g.y = (msg[2] * 256 + msg[3])
            self.g.z = (msg[4] * 256 + msg[5])

            if(self.g.x > 32768):
                self.g.x -= 65536
            if(self.g.y > 32768):
                self.g.y -= 65536
            if(self.g.z > 32768):
                self.g.z -= 65536

            self.g.x /= 100
            self.g.y /= 100
            self.g.z /= 100

            self.g.time = timestamp

            modifiedSensors.append(self.g)
            #modifiedSensors.append(self.g.type)

        if(id == 0xC0):
            # STEER
            if(msg[0] == 0x02):
                self.steer.angle = (msg[1] * 256 + msg[2]) / 100
                self.steer.time = timestamp

                modifiedSensors.append(self.steer)
                #modifiedSensors.append(self.steer.type)

        if(id == 0xD0):
            # ENCODERS
            self.l_enc.rads = ((msg[0] * (256 * 256)) + (msg[1] * 256) + msg[2]) / 10000
            self.r_enc.rads = ((msg[3] * (256 * 256)) + (msg[4] * 256) + msg[5]) / 10000

            if(msg[6] == 1):
                self.l_enc.rads *= -1
            if(msg[7] == 1):
                self.r_enc.rads *= -1

            self.l_enc.time = timestamp
            self.r_enc.time = timestamp

            modifiedSensors.append(self.l_enc)
            modifiedSensors.append(self.r_enc)
            #modifiedSensors.append(self.l_enc.type)
            #modifiedSensors.append(self.r_enc.type)

        if(id == 0xD1):
            self.l_enc.rotations = (msg[0] * (256 * 256)) + (msg[1] * 256) + msg[2]
            self.l_enc.km = (msg[3] * (256 * 256)) + (msg[4] * 256) * msg[5]

            self.r_enc.rotations = (msg[0] * (256 * 256)) + (msg[1] * 256) + msg[2]
            self.r_enc.km = (msg[3] * (256 * 256)) + (msg[4] * 256) * msg[5]

            modifiedSensors.append(self.l_enc)
            modifiedSensors.append(self.r_enc)

        if(id == 0xAA):
            if(msg[0] == 0x01):
                self.bmsHV.voltage = ((msg[1] * (256 * 256)) + (msg[2] * 256) + msg[3]) / 10000
                self.bmsHV.max_voltage = ((msg[4] * 256) + msg[5]) / 10000
                self.bmsHV.min_voltage = ((msg[6] * 256) + msg[7]) / 10000
                self.bmsHV.time = timestamp

                modifiedSensors.append(self.bmsHV)
                #modifiedSensors.append(self.bmsHV.type)
            elif(msg[0] == 0x03):
                self.bmsHV_state.value = "TS_ON"
                self.bmsHV_state.time = timestamp

                modifiedSensors.append(self.bmsHV_state)
                #modifiedSensors.append(self.bmsHV_state.type)
            elif(msg[0] == 0x04):
                self.bmsHV_state.value = "TS_OFF"
                self.bmsHV_state.time = timestamp

                modifiedSensors.append(self.bmsHV_state)
                #modifiedSensors.append(self.bmsHV_state.type)
            elif(msg[0] == 0x05):
                self.bmsHV.current = ((msg[1] * 256) + msg[2]) / 10
                self.bmsHV.power = (msg[3] * 256) + msg[4]

                modifiedSensors.append(self.bmsHV)
                #modifiedSensors.append(self.bmsHV.type)
            elif(msg[0] == 0x08):
                self.bmsHV_state.value = BMS_ERRORS[msg[1]] + " - > " + str(msg[2])
                self.bmsHV_state.time = timestamp

                modifiedSensors.append(self.bmsHV_state)
                #modifiedSensors.append(self.bmsHV_state.type)
            elif(msg[0] == 0x09):
                self.bmsHV_state.value = BMS_WARNINGS[msg[1]] + " - > " + str(msg[2])
                self.bmsHV_state.time = timestamp

                modifiedSensors.append(self.bmsHV_state)
                #modifiedSensors.append(self.bmsHV_state.type)
            elif(msg[0] == 0x0A):
                self.bmsHV.temperature = ((msg[1] * 256) + msg[2]) / 100
                self.bmsHV.max_temperature = ((msg[3] * 256) + msg[4]) / 100
                self.bmsHV.min_temperature = ((msg[5] * 256) + msg[6]) / 100
                self.bmsHV.time = timestamp

                modifiedSensors.append(self.bmsHV)
                #modifiedSensors.append(self.bmsHV.type)

        # ECU
        if(id == 0x55):
            # ECU State
            if(msg[0] == 0x01):
                self.ecu_state.value = "STATE: " + ECU_STATES[msg[4]] + " Map: " + str(msg[3])
            # ECU bms on request
            elif(msg[0] == 0x0B and msg[1] == 0x04):
                self.ecu_state.value = "REQUEST TS OFF"
            elif(msg[0] == 0x0B and msg[1] == 0x08):
                self.ecu_state.value = "REQUEST TS OFF ERRORS"
            elif(msg[0] == 0x0A):
                self.ecu_state.value = "REQUEST TS ON"

            ## CHIEDERE A PHIL
            self.ecu.time = timestamp

            modifiedSensors.append(self.ecu_state)
            #modifiedSensors.append(self.ecu_state.type)

        # STEERING
        if(id == 0xA0):
            if(msg[0] == 0x03):
                self.steeringWheel.value = "REQUEST TS ON"
                #self.cmds.push_one(("Steering Setup request", timestamp))
            elif(msg[0] == 0x04):
                self.steeringWheel.value = "REQUEST TO IDLE"
                #self.cmds.push_one(("Steering Stop request", timestamp))
            elif(msg[0] == 0x05):
                self.steeringWheel.value = "REQUEST TO RUN"
                #self.cmds.push_one(("Steering RUN request", timestamp))
            elif(msg[0] == 0x06):
                self.steeringWheel.value = "REQUEST TO SETUP"
            elif(msg[0] == 0x08):
                self.steeringWheel.value = "REQUEST INVERTER LEFT ON"
            elif(msg[0] == 0x09):
                self.steeringWheel.value = "REQUEST INVERTER RIGHT ON"

            ## CHIEDERE A PHIL
            self.steeringWheel.time = timestamp
            modifiedSensors.append(self.steeringWheel)
            #modifiedSensors.append(self.steeringWheel.type)

        if(id == 0x201):
            if(msg[0] == 0x90):
                self.ecu.power_request_left = (msg[2] * 256) + msg[1]
                self.ecu.time = timestamp

                modifiedSensors.append(self.ecu)
                #modifiedSensors.append(self.ecu.type)

        if(id == 0x202):
            if(msg[0] == 0x90):
                self.ecu.power_request_right = (msg[2] * 256) + msg[1]
                self.ecu.time = timestamp

                modifiedSensors.append(self.cmds)
                #modifiedSensors.append(self.cmds.type)

        # BMS LV
        if(id == 0xFF):
            self.bmsLV.voltage = msg[0] / 10
            self.bmsLV.temperature = msg[2] / 5
            self.bmsLV.max_temperature = msg[3] / 5
            self.bmsLV.time = timestamp

            modifiedSensors.append(self.bmsLV)
            #modifiedSensors.append(self.bmsLV.type)

        # INVERTER LEFT
        if(id == 0x181):
            if(msg[0] == 0xA0):
                self.invl.torque = (msg[2] * 256 + msg[1])
                if(self.invl.torque > 32768):
                    self.invl.torque -= 65536
                self.invl.time = timestamp
            elif(msg[0] == 0x4A):
                self.invl.temperature = (msg[2] * 256 + msg[1] - 15797) / 112.1182
                self.invl.time = timestamp
            elif(msg[0] == 0x49):
                self.invl.motor_temp = (msg[2] * 256 + msg[1] - 9393.9) / 55.1
                self.invl.time = timestamp
            elif(msg[0] == 0xA8):
                self.invl.speed = (msg[2] * 256 + msg[1])
                if(self.invl.speed > 32768):
                    self.invl.speed -= 65536
                self.invl.time = timestamp

            ## CHIEDERE A PHIL
            modifiedSensors.append(self.invl)
            #modifiedSensors.append(self.invl.type)

        # INVERTER RIGHT
        if(id == 0x182):
            if(msg[0] == 0xA0):
                self.invr.torque = (msg[2] * 256 + msg[1])
                if(self.invr.torque > 32768):
                    self.invr.torque -= 65536
                self.invr.time = timestamp
            elif(msg[0] == 0x4A):
                self.invr.temperature = (msg[2] * 256 + msg[1] - 15797) / 112.1182
                self.invr.time = timestamp
            elif(msg[0] == 0x49):
                self.invr.motor_temp = (msg[2] * 256 + msg[1] - 9393.9) / 55.1
                self.invr.time = timestamp
            elif(msg[0] == 0xA8):
                self.invr.speed = (msg[2] * 256 + msg[1])
                if(self.invr.speed > 32768):
                    self.invr.speed -= 65536
                self.invr.time = timestamp

            ## CHIEDERE A PHIL
            modifiedSensors.append(self.invr)
            #modifiedSensors.append(self.invr.type)

        return modifiedSensors


    def fill_GPS(self, timestamp, type, payload):
        modified = False
        time_ = timestamp

        if(self.__count_empty_elements(payload) > 3):
            return modified

        self.gps.clear()

        if("GGA" in type):

            self.gps.timestamp = float(payload[0])

            self.gps.latitude = float(payload[1])
            self.gps.longitude = float(payload[3])

            self.gps.altitude = float(payload[8])

            self.gps.time = time_

            self.gps.convert_latitude()
            self.gps.convert_longitude()

            modified = True

        if("GLL" in type):

            self.gps.latitude = float(payload[0])
            self.gps.longitude = float(payload[2])

            self.gps.timestamp = float(payload[4])

            self.gps.time = time_

            self.gps.convert_latitude()
            self.gps.convert_longitude()

            modified = True

        if("RMC" in type):

            self.gps.timestamp = float(payload[0])

            self.gps.latitude = float(payload[2])
            self.gps.longitude = float(payload[4])

            self.gps.speed = float(payload[6])
            self.gps.course = float(payload[7])

            self.gps.time = time_

            self.gps.convert_latitude()
            self.gps.convert_longitude()

            modified = True

        return modified

    def __count_empty_elements(self, list):
        count = 0

        for e in list:
            if e == "":
                count += 1

        return count

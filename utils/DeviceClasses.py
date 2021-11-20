class Accel_Gyro(object):
    type = "Accel Gyro"

    def __init__(self):
        self.x = 0
        self.y = 0
        self.z = 0
        self.scale = 0

        self.jsonList = []
        self.messageList = []

        self.time = 0

    def get_obj(self):
        obj = [
            self.x,
            self.y,
            self.z,
            self.scale
        ]
        names = [
            "x",
            "y",
            "z",
            "scale"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "x": self.x,
            "y": self.y,
            "z": self.z,
            "scale": self.scale
        }
        return _dict


class Encoder:
    type = "Encoder"

    def __init__(self):
        self.rads = 0
        self.rotation = 0
        self.km = 0
        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.rads,
            self.rotation,
            self.km
        ]
        names = [
            "rads",
            "rotation",
            "km"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "rads": self.rads,
            "rotation": self.rotation,
            "km": self.km
        }
        return _dict


class Speed:
    type = "Speed"

    def __init__(self):
        self.l_kmh = 0
        self.r_kmh = 0
        self.l_rads = 0
        self.r_rads = 0
        self.angle0 = 0
        self.angle1 = 0
        self.delta = 0
        self.frequency = 0
        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.l_kmh,
            self.r_kmh,
            self.l_rads,
            self.r_rads,
            self.angle0,
            self.angle1,
            self.delta,
            self.frequency
        ]
        names = [
            "l_kmh",
            "r_kmh",
            "l_rads",
            "r_rads",
            "angle0",
            "angle1",
            "delta",
            "frequency"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "l_kmh": self.l_kmh,
            "r_kmh": self.r_kmh,
            "l_rads": self.l_rads,
            "r_rads": self.r_rads,
            "angle0": self.angle0,
            "angle1": self.angle1,
            "delta": self.delta,
            "frequency": self.frequency
        }
        return _dict


class Steer:
    type = "Steer"

    def __init__(self):
        self.angle = 0
        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.angle
        ]
        names = [
            "angle"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "angle": self.angle
        }
        return _dict


class Pedals:
    type = "Pedals"

    def __init__(self):
        self.throttle1 = 0
        self.throttle2 = 0
        self.front = 0
        self.back = 0
        self.brake = 0
        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.throttle1,
            self.throttle2,
            self.front,
            self.back,
            self.brake
        ]
        names = [
            "throttle1",
            "throttle2",
            "front",
            "back",
            "brake"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "throttle1": self.throttle1,
            "throttle2": self.throttle2,
            "front": self.front,
            "back": self.back,
            "brake": self.brake
        }
        return _dict


class ECU:
    type = "ECU"

    def __init__(self):
        self.errors = 0
        self.warnings = 0
        self.state = 0
        self.map = 0

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.errors,
            self.warnings,
            self.state,
            self.map
        ]
        names = [
            "errors",
            "warnings",
            "state",
            "map"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "errors": self.errors,
            "warnings": self.warnings,
            "state": self.state,
            "map": self.map
        }
        return _dict


class ECU_state:
    type = "State ECU"

    def __init__(self):
        self.ok = 0

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.ok
        ]
        names = [
            "ok"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "ok": self.ok
        }
        return _dict


class SteeringWheel:
    type = "SteeringWheel"

    def __init__(self):
        self.ok = 0

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.ok
        ]
        names = [
            "ok"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "ok": self.ok
        }
        return _dict


class Commands:
    type = "Commands"

    def __init__(self):
        self.active_commands = []

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        return self.active_commands, "list of active commands"

    def push_one(self, command):
        elements = list(
            filter(lambda x: command[0] in x, self.active_commands))
        if len(elements) > 0:
            idx = self.active_commands.index(elements[0])
            self.active_commands[idx] = command
        else:
            self.active_commands.append(command)

    def remove_command(self, idx=0):
        self.active_commands.pop(idx)

    def clear(self):
        self.active_commands = []

    def get_dict(self):
        _dict = {
            "active_commands": self.active_commands
        }

        return _dict


class Inverter:
    type = "Inverter"

    def __init__(self):
        self.temperature = 0
        self.motorTemp = 0
        self.torque = 0
        self.speed = 0
        self.state = 0

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.temperature,
            self.motorTemp,
            self.torque,
            self.speed,
            self.state
        ]
        names = [
            "temperature",
            "motorTemp",
            "torque",
            "speed",
            "state"
        ]

        return obj, names

    def get_dict(self):
        _dict = {
            "temperature": self.temperature,
            "motorTemp": self.motorTemp,
            "torque": self.torque,
            "speed": self.speed,
            "state": self.state
        }
        return _dict


class BMS:
    type = "BMS"

    def __init__(self):
        self.temperature = 0
        self.voltage = 0
        self.current = 0
        self.power = 0

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.temperature,
            self.voltage,
            self.current,
            self.power
        ]
        names = [
            "temperature",
            "voltage",
            "current",
            "power"
        ]

        return obj, names

    def get_dict(self):
        _dict = {
            "temperature": self.temperature,
            "voltage": self.voltage,
            "current": self.current,
            "power": self.power
        }
        return _dict


class BMS_state:
    type = "State BMS"

    def __init__(self):
        self.ok = 0

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.ok
        ]
        names = [
            "ok"
        ]
        return obj, names

    def get_dict(self):
        _dict = {
            "ok": self.ok
        }
        return _dict


class GPS:
    type = "GPS"

    def __init__(self):
        self.latitude = 0
        self.longitude = 0
        self.altitude = 0
        self.speed = 0
        self.course = 0
        self.timestamp = 0

        self.time = 0

        self.jsonList = []
        self.messageList = []

    def get_obj(self):
        obj = [
            self.timestamp,
            self.latitude,
            self.longitude,
            self.altitude,
            self.speed,
            self.course
        ]
        names = [
            "timestamp",
            "latitude",
            "longitude",
            "altitude",
            "speed",
            "course"
        ]

        return obj, names

    def get_dict(self):
        _dict = {
            "timestamp": self.timestamp,
            "latitude": self.latitude,
            "longitude": self.longitude,
            "altitude": self.altitude,
            "speed": self.speed,
            "course": self.course
        }

        return _dict

    def clear(self):
        self.latitude = 0
        self.longitude = 0
        self.altitude = 0
        self.speed = 0
        self.course = 0
        self.timestamp = 0

    def convert_latitude(self):
        lat_degree = int(self.latitude / 100)

        lat_mm_mmmm = self.latitude % 100

        self.latitude = lat_degree + (lat_mm_mmmm / 60)

    def convert_longitude(self):

        lng_degree = int(self.longitude / 100)

        lng_mm_mmmmm = self.longitude % 100

        self.longitude = lng_degree + (lng_mm_mmmmm / 60)

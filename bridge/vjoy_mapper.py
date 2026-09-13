from ast import arg
from multiprocessing import Value
import threading
import pyvjoy
import serial
import serial.tools.list_ports

VJOY_AXIS_MAX=0x8000
VJOY_AXIS_MIN=0x1
VJOY_AXIS_REST=0x4000

WHEEL_COM_BAUD=115200
WHEEL_ROT_PULSE=0.03    # 30ms rotary encoder down->up pulse


BRIDGE_ENABLED=True
BRIDGE_PORT="COM20"        # the com0com port NOT used by SimHub
BRIDGE_BAUD=115200         # match whatever baud SimHub is configured to use
BRIDGE_READ_CHUNK=256


def menu_selection():
    while True :
        ports=serial.tools.list_ports.comports()
        
        print("\nSelect one of the following devices, or update the list:")
        print("    [0] Update list")

        for i, port in enumerate(ports):
            print(f"    [{i+1}] {port.device}: {port.description}")

        try:
            selection=int(input())

            if 0<selection<=len(ports) :
                return ports[selection-1].device
            elif selection==0 :
                continue

        except ValueError:
            pass

def map_range(value: float, in_min: float, in_max: float, out_min: float, out_max: float):
    return int(round((value-in_min)*(out_max-out_min)/(in_max-in_min)+out_min))

def trigger_btn_pulse(wheel: pyvjoy.VJoyDevice, btn: int):
    wheel.set_button(btn, 1)

    timer=threading.Timer(WHEEL_ROT_PULSE, lambda: wheel.set_button(btn, 0))
    timer.daemon=True
    timer.start()

def process_line(line: str, wheel: pyvjoy.VJoyDevice):
    splitted=line.split("|")

    match splitted[0]:
        case "CO":

            match splitted[1]:
                case "rot":
                    if splitted[2]=="cw":
                        trigger_btn_pulse(wheel, 1)
                    elif splitted[2]=="ccw":
                        trigger_btn_pulse(wheel, 2)
                    

                case "btn":
                    if splitted[3]=="down":
                        state=True
                    elif splitted[3]=="up":
                        state=False
                    else:
                        state=False

                    wheel.set_button(int(splitted[2])+3, state)     # +2 to address the 0 and 1 buttons

                case "steer":
                    value=int(splitted[2])
                    wheel.set_axis(pyvjoy.HID_USAGE_X, map_range(value, 0, 1023, VJOY_AXIS_MIN, VJOY_AXIS_MAX))

                case _:
                    print("Unknown control message")

        case _:
            print("Unknown serial message")
          
def init_vjoy_values(wheel: pyvjoy.VJoyDevice):
    wheel.set_axis(pyvjoy.HID_USAGE_X, VJOY_AXIS_REST)
    wheel.set_axis(pyvjoy.HID_USAGE_Y, VJOY_AXIS_REST)
    wheel.set_axis(pyvjoy.HID_USAGE_Z, VJOY_AXIS_REST)

def process_simhub(virtual_port: str, r_ser: serial.Serial, thread_stop: threading.Event):
    try:
        v_ser=serial.Serial(virtual_port, BRIDGE_BAUD)

    except serial.SerialException:
        print(f"[SimHub bridge] Cannot open com0com port {BRIDGE_PORT}. Continue without bridge.")
        return

    print(f"SimHub bridge has been connected to com0com port {BRIDGE_PORT}.")

    try:
        while not thread_stop.is_set():
            line=v_ser.readline().decode().strip()
            try:
                r_ser.write(f"{line}\r\n".encode())
            except serial.SerialException:
                print(f"[SimHub bridge] Communication error.")

    finally:
        v_ser.close()
        print("[SimHub bridge] Exiting.")

def main():
    thread_stop=threading.Event()
    com0com_thread=None     # to not close the thread forcefully

    try:
        wheel=pyvjoy.VJoyDevice(1)
        wheel.reset()
        init_vjoy_values(wheel)

        port=menu_selection()
        ser=serial.Serial(port, WHEEL_COM_BAUD)
        print(f"\nConnected to {ser.name}")

        if BRIDGE_ENABLED:
            com0com_thread=threading.Thread(target=process_simhub, args=(BRIDGE_PORT, ser, thread_stop), daemon=True)
            com0com_thread.start()

        while True:
            process_line(ser.readline().decode().strip(), wheel)

    except (KeyboardInterrupt, serial.SerialException):
        try:
            print("Disconnecting...")

            thread_stop.set()
            if com0com_thread is not None:
                com0com_thread.join(timeout=1)

            wheel.reset()
            ser.close()
        except UnboundLocalError:
            pass

main()
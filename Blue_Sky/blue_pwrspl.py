from ophyd import Component, Device, EpicsSignal, EpicsSignalRO
from ophyd.status import Status

class pwrspl(Device):
    # Señales de lectura (readback)
    voltage_RB  = Component(EpicsSignalRO, "PWRSPL:readV")
    current_RB  = Component(EpicsSignalRO, "PWRSPL:readI")

    # Señal de setpoint
    current_SP  = Component(EpicsSignal,   "PWRSPL:setI")

    # Señales de encendido/apagado
    on_cmd      = Component(EpicsSignal,   "PWRSPL:on")
    off_cmd     = Component(EpicsSignal,   "PWRSPL:off")
    # Estado de salida (asumimos que `PWRSPL:on` devuelve 1 cuando está on)
    onoff_status= Component(EpicsSignalRO, "PWRSPL:on")

    def set(self, setpoint):
        stat = Status(timeout=30.0)
        print(f"onoff stat: {self.onoff_status.get()}")
        # Asegurar que esté encendido
        if self.onoff_status.get() == "":
          self.on_cmd.put("1")

        # Poner el setpoint
        self.current_SP.put(setpoint)

        # Esperar hasta que el readback y el setpoint coincidan (±0.1)
        while True:
            if abs(self.current_RB.get() - self.current_SP.get()) < 0.1:
                stat.set_finished()
                break

        return stat

    def toff(self):
        stat = Status(timeout=30.0)
        print(f"onoff stat: {self.onoff_status.get()}")
        # Asegurar que esté encendido
        if self.onoff_status.get() == "1":
          self.on_cmd.put("")

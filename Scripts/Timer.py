import tkinter as tk
from datetime import *
import calendar
from dateutil.relativedelta import *

class TimerInfo:
    column = 0
    def __init__(self, window, name, text, bg):
        self._window = window
        self._name = name

        # create the frame and label
        self._frame = tk.Frame(master=window)
        self._label = tk.Label(master=self._frame, text=text, font=("Arial", 25), foreground="white", background=bg)
        self._label.pack()
        self._frame.grid(row = 0, column=TimerInfo.column)

        self._labelFrame = tk.Frame(master=window)
        self._labelLabel = tk.Label(master=self._frame, text=name, font=("Arial", 25), foreground="white", background=bg)
        self._labelLabel.pack()
        self._frame.grid(row=1, column=TimerInfo.column)
        TimerInfo.column += 1

        
class Timer:
    def __init__(self, title):
        self._window = tk.Tk()
        self._window.title(title)
        self._counter = 0
        self._fields = { 
            'year':    TimerInfo(self._window, 'YEARS ',    '0000', 'blue'),
            'months':  TimerInfo(self._window, 'MONTHS',  '00', 'green'),
            'days':    TimerInfo(self._window, 'DAYS  ',    '00', 'blue'),
            'hours':   TimerInfo(self._window, 'HRS   ',   '00', 'green'),
            'minutes': TimerInfo(self._window, 'MINS  ', '00', 'blue'),
            'seconds': TimerInfo(self._window, 'SECS  ', '00', 'green'),
        }
        self._window.after(1000, self.update)

        # retirement date
        self._finalDate = datetime.fromisoformat('2029-09-30')

        self._window.mainloop()

    def update(self):
        r = relativedelta(self._finalDate, datetime.now())
        self._fields['seconds']._label.config(text='{seconds:2d}'.format(seconds=r.seconds))
        self._fields['minutes']._label.config(text='{minutes:2d}'.format(minutes=r.minutes))
        self._fields['hours']._label.config(text='{hours:2d}'.format(hours=r.hours))
        self._fields['days']._label.config(text='{days:2d}'.format(days=r.days))
        self._fields['months']._label.config(text='{months:2d}'.format(months=r.months))
        self._fields['year']._label.config(text='{year:4d}'.format(year=r.years))
        self._window.after(1000, self.update)

timer1 = Timer('Amir S. Ansari Retirement Time')


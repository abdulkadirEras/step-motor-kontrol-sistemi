2.4 Inch Dwin HMI Display Epressif firmasının ESP32 denetleyicisi kullanarak 7 tane step motoru saat yönüne/tersine 
ve motorun hızını kontrol etmeye yarayan sistem tasarlanmıştır.

Sistemin yazılımı Cpp ile donanım tasarımı KiCad’ de tasarlanmıştır.

Kullanıcı ara yüzü ile operatör manuel olarak kontrol edebildiği gibi operatörün ayarladığı sürelerde istediği motor 
da otomatik çalışabiliyor.

Sistemde güç butonları step motor sürücüleri aktif veya pasif yapılabilir. Aktif veya pasif durumları 
EEPROM’ da kaydediliyor.

Ekrandaki Slide Bar ile Step Motorların hızları ayarlanabilir.
Record butonu ile de operatör motorların çalışma sürelerini ve motor dönüş yönlerini kaydederek otomatik olarak çalıştırabilir.

Operatörün kaydettiği çalışma şeklini soldaki ekrandan play butonuna dokunarak pause butonuna basana kadar çalıştırabilir. 

Acil stop butonuna operatör bastığında ise sistem tüm step motorlarını durdurur.


A control system was designed using a 2.4 Inch Dwin HMI Display and an Espressif ESP32 microcontroller to regulate the speed and direction (clockwise/counter-clockwise) of seven stepper motors.

The system's software was developed using C++, and the hardware was designed in KiCad.

Via the user interface, the operator can not only control the motors manually but also enable automatic operation for specific motors at durations set by the operator.

The system includes power buttons that allow the stepper motor drivers to be actively turned on or off. Their active/inactive status is saved in EEPROM.

Motor speeds can be adjusted using the Slide Bar on the display. Furthermore, the Record button allows the operator to save custom sequences of motor run times and rotation directions for automatic playback.

The operator can execute the recorded sequence by tapping the Play button on the left side of the screen, and the sequence will run until the Pause button is pressed.

If the operator presses the Emergency Stop button, the system immediately halts all stepper motors.

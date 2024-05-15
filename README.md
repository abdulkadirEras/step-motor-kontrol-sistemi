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

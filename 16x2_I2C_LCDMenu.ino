#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
//Butonların giriş pinleri
#define sag 2
#define sol 3
#define yukari 4
#define asagi 5
#define tamam 6

//===Test Amaçlı====
#define kirmiziPin 9
#define yesilPin 10
#define maviPin 11
//==================

//Hareket tanımlamaları
#define saga 6
#define sola 4
#define yukariya 8
#define asagiya 2
#define tamama 5
#define hareketYok 0

//Butona basılma durumlarının tutulacağı değişkenler
int sag_durum = 0;
int sol_durum = 0;
int yukari_durum = 0;
int asagi_durum = 0;
int tamam_durum = 0;
int buton_durum = 0;

//Menu Id leri
//Menu ekleme yada değişiklik yapma işine burada başlamanız gerekiyor
//Yeni menü elemanı eklenecek ise verdiğiniz değer aynı olmayacak şekilde aşağıda bir tanımlama yapmalısınız
//Örn: "const int menuYeni=20;
//Daha sonra Setup kısmında kaydını yapmalısınız
//                1          2            3        4    5     6   7   8
//"menuEkle(menuAnaMenu, menuYeni,  "Yeni Menu", true, true, 189, 0, 255);"
//1)Üst menü elemanı, yeni menü elemanı hangi menü elemanının altında görünecek
//2)Yeni oluşturduğumuz tanımlama
//3)Menü elemanının görünecek adı
//4)Altında başka menü elemanları olmayacak ve bir değer tutacaksa (program içerisinde bu değeri kullanabilirisiniz , "menuEl(menuYeni).gecerliDeger" şeklinde erişim sağlayabilirsiniz) "true" değilse "false"
//5)Eproma kayıt edilecek mi?
//6)İlk çalıştığında yada Eprom daki değer sıfır ise bir değer vermek istiyebilirsiniz, programı kullanarak değiştirdiğinizde ARDUINO içerisindeki EPROM a kayıt yapacağı için tanımladığınızı değil EPROM daki değeri getirecektir.
//7)Girilebilecek en düşük değer
//8)Girilebilecek en yüksek değer (Eprom a yapılan kayıt byte türünden olduğu için en yüksek 255 verebilirsiniz)
const byte menuAnaMenu = 0;
const byte menuSaatAyarla = 2;
const byte menuTarihAyarlar = 3;
const byte menuEkranAyarlari = 4;
const byte menuKontrast = 5;
const byte menuParlaklik = 6;
const byte menuOranlar = 7;
const byte menuEn = 8;
const byte menuBoy = 9;
const byte menuYukseklik = 10;
const byte menuRenk = 11;
const byte menuMavi = 12;
const byte menuKirmizi = 13;
const byte menuYesil = 14;
const byte menuSaat = 15;
const byte menuDakika = 16;
const byte menuTarihGun = 17;
const byte menuTarihAy = 18;
const byte menuTarihYil = 19;
const byte menuEkranBeklemeSuresi = 20;
//Menuler bu yapıya göre oluşturuluyor
struct menuElemani {
  byte id;
  byte ustId;
  String menu;
  bool degerVar;
  bool eprom;
  byte gecerliDeger;
  byte enFazla;
  byte enDusuk;
};
//Menü elemanlarının tablosu diyebiliriz, yukarıda tanımlanan menü elemanları artıklca buradaki sayıyı da arttırmanız gerekiyor. Örn:"menuElemanlari[30];"
struct menuElemani menuElemanlari[20];

//Ekranda görünen menunun bilgisi
byte gosterilenMenuId = 0;
//Değer değişikliği yapılacak menu bilgisi
byte seciliMenuId = 0;
//Tuşun art arda basma süresinin kontrol etmek ve ana ekrana dönmek için
unsigned long eskiZaman = 0;
bool zamanKontrol = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(sag, INPUT);
  pinMode(sol, INPUT);
  pinMode(yukari, INPUT);
  pinMode(asagi, INPUT);
  pinMode(tamam, INPUT);

  //====Test Amaçlı====
  pinMode(kirmiziPin, OUTPUT);
  pinMode(yesilPin, OUTPUT);
  pinMode(maviPin, OUTPUT);
  //===================

  lcd.init();
  eskiZaman = millis();
  zamanKontrol = true;
  lcd.backlight();

  //Menu yapısı oluşturuluyor, menu elemanlarının tablosu dolduruluyor da diyebiliriz

  menuEkle(menuAnaMenu, menuSaatAyarla, "Saat Ayarla", false, true, 0, 0, 0);
  menuEkle(menuAnaMenu, menuTarihAyarlar,  "Tarih Ayarla", false, true, 0, 0, 0);
  menuEkle(menuAnaMenu, menuEkranAyarlari,  "Ekran Ayarlari", false, true, 0, 0, 0);
  menuEkle(menuAnaMenu, menuOranlar, "Oranlar", false, true, 0, 0, 0);
  menuEkle(menuAnaMenu, menuRenk,  "Renkler", false, true, 0, 0, 0);
  menuEkle(menuAnaMenu, menuEkranBeklemeSuresi,  "Ekran Suresi", true, true, 1, 1, 20);

  menuEkle(menuSaatAyarla, menuSaat,  "Saat", true, true, 0, 0, 23);
  menuEkle(menuSaatAyarla, menuDakika,  "Dakika", true, true, 0, 0, 59);

  menuEkle(menuTarihAyarlar, menuTarihGun,  "Gun", true, true, 1, 1, 31);
  menuEkle(menuTarihAyarlar, menuTarihAy,  "Ay", true, true, 1, 1, 12);
  menuEkle(menuTarihAyarlar, menuTarihYil,  "Yil", true, true, 20, 20, 99);

  menuEkle(menuEkranAyarlari, menuKontrast,  "Kontrast", true, true, 0, 0, 20);
  menuEkle(menuEkranAyarlari, menuParlaklik, "Parlaklik", true, true, 0, 0, 20);

  menuEkle(menuOranlar, menuEn,  "En", true, true, 50, 0, 100);
  menuEkle(menuOranlar, menuBoy, "Boy", true, true, 100, 0, 100);
  menuEkle(menuOranlar, menuYukseklik, "Yukseklik", true, true, 80, 0, 100);

  menuEkle(menuRenk, menuMavi,  "Mavi", true, true, 0, 0, 255);
  menuEkle(menuRenk, menuKirmizi,  "Kirmizi", true, true, 0, 0, 255);
  menuEkle(menuRenk, menuYesil,  "Yesil", true, true, 0, 0, 255);

  //Yeni menü elemanı eklendiğinde buradan kayıt yapmalısınız
  //Örn:"menuEkle(menuAnaMenu, menuYeni,  "Yeni Menu", true, true, 189, 0, 255);"
  
  EkranOlustur();
}

byte mIndex = 0;
void menuEkle(byte uid, byte id,  String m, bool d, bool e, byte gd, byte ed, byte ef)
{
  menuElemanlari[mIndex].id = id;
  menuElemanlari[mIndex].ustId = uid;
  menuElemanlari[mIndex].menu = m;
  menuElemanlari[mIndex].degerVar = d;
  menuElemanlari[mIndex].eprom = e;
  menuElemanlari[mIndex].enFazla = ef;
  menuElemanlari[mIndex].enDusuk = ed;

  if (e)
  {
    //Menü Id si Eprom adresi olarak kullanılıyor
    menuElemanlari[mIndex].gecerliDeger = EEPROM.read(id);
    if (menuElemanlari[mIndex].gecerliDeger == "\n" || menuElemanlari[mIndex].gecerliDeger == 0)
      menuElemanlari[mIndex].gecerliDeger = gd;
    if (menuElemanlari[mIndex].gecerliDeger < ed)
      menuElemanlari[mIndex].gecerliDeger = ed;
    if (menuElemanlari[mIndex].gecerliDeger > ef)
      menuElemanlari[mIndex].gecerliDeger = ef;
  }
  else
    menuElemanlari[mIndex].gecerliDeger = gd;
  mIndex++;
}
//Döngülerde kullanılacak üst değer için
byte menuAdedi()
{
  return mIndex;
}
//Menu Id sine göre array in index ini buluyor
byte bulMenuIndex(byte Id)
{
  for (byte i = 0; i < menuAdedi(); i++)
    if (menuElemanlari[i].id == Id)
      return i;
  return -1;
}

//Ekranı dolduran fonksiyon
void EkranOlustur()
{
  lcd.clear();
  if (gosterilenMenuId == 0)
  {
    lcd.print("Hos Geldiniz");
    lcd.setCursor(0, 1);
    lcd.print("Sag Tusa Basin >");
  }
  else
  {
    byte menuIndex = bulMenuIndex(gosterilenMenuId);
    byte ustMenuIndex = 0;
    if (menuElemanlari[menuIndex].ustId != 0)
    {
      ustMenuIndex = bulMenuIndex(menuElemanlari[menuIndex].ustId);
      lcd.print(menuElemanlari[ustMenuIndex].menu);
    }
    if (seciliMenuId != 0)
    {
      lcd.setCursor(15, 0);
      lcd.print("*");
    }
    lcd.setCursor(0, 1);
    lcd.print(menuElemanlari[menuIndex].menu);
    if (altMenusuVarmi(gosterilenMenuId))
      lcd.print(">");
    else if (menuElemanlari[menuIndex].degerVar)
    {
      lcd.setCursor(13, 1);
      if (menuElemanlari[menuIndex].gecerliDeger < 10)
        lcd.print("00");
      else if (menuElemanlari[menuIndex].gecerliDeger < 100)
        lcd.print("0");
      lcd.print(menuElemanlari[menuIndex].gecerliDeger);
    }
  }
}
//Menu elemanının eltında tanımlı menu elemanı varsa true döner
bool altMenusuVarmi(byte menuId)
{
  for (byte i = 0; i < menuAdedi(); i++)
    if (menuElemanlari[i].ustId == menuId)
      return true;
  return false;
}

void loop() {
  sag_durum = digitalRead(sag);
  sol_durum = digitalRead(sol);
  yukari_durum = digitalRead(yukari);
  asagi_durum = digitalRead(asagi);
  tamam_durum = digitalRead(tamam);
  DurumDegerlendir();
  RenkAyarla();
  HareketsizKalmaKontrolu();
}

//Menü seçimi yapılmamış ve belirtilen dakika içinde işlem yapılmamışsa ekranı kapatır ve ana ekrana döner
void HareketsizKalmaKontrolu()
{
  unsigned long yeniZaman = millis();
  if (seciliMenuId == 0 && zamanKontrol && yeniZaman - eskiZaman > (menuEl(menuEkranBeklemeSuresi).gecerliDeger * 60000))
  {
    gosterilenMenuId = 0;
    zamanKontrol = false;
    lcd.noBacklight();
    EkranOlustur();
  }
}

//===Test İçin====
byte kirmizi, yesil, mavi = 0;
void RenkAyarla()
{
  if (menuEl(menuKirmizi).gecerliDeger != kirmizi || menuEl(menuYesil).gecerliDeger != yesil || menuEl(menuMavi).gecerliDeger != mavi)
  {
    kirmizi = menuEl(menuKirmizi).gecerliDeger;
    yesil = menuEl(menuYesil).gecerliDeger;
    mavi = menuEl(menuMavi).gecerliDeger;
    analogWrite(kirmiziPin, kirmizi);
    analogWrite(yesilPin, yesil);
    analogWrite(maviPin, mavi);
  }
}
//================

//Menu Id ye göre menu elemanı döner
struct menuElemani menuEl(byte id)
{
  byte menuIndex = bulMenuIndex(id);
  return menuElemanlari[menuIndex];
}

//Yukarı tuşuna basıldığında gösterilecek menü yü ayarlar
void menuYukari()
{
  byte menuIndex = bulMenuIndex(gosterilenMenuId);
  //if (menuElemanlari[menuIndex].ustId != 0)
  //{
  byte m[menuAdedi()];
  byte mIx = 0;
  byte umId = menuElemanlari[menuIndex].ustId;
  byte gI = 0;
  for (byte i = 0; i < menuAdedi(); i++)
  {
    if (menuElemanlari[i].ustId == umId)
    {
      m[mIx] = i;
      if (menuIndex == i)
        gI = mIx;
      mIx++;
    }
  }
  if (gI == 0)
    gI = mIx - 1;
  else
    gI--;
  gosterilenMenuId = menuElemanlari[m[gI]].id;
  //}
}
//Aşağı tuşuna basıldığında gösterilecek menüyü ayarlar
void menuAsagi()
{
  byte menuIndex = bulMenuIndex(gosterilenMenuId);
  //if (menuElemanlari[menuIndex].ustId != 0)
  //{
  byte m[menuAdedi()];
  byte mIx = 0;
  byte umId = menuElemanlari[menuIndex].ustId;
  byte gI = 0;
  for (byte i = 0; i < menuAdedi(); i++)
  {
    if (menuElemanlari[i].ustId == umId)
    {
      m[mIx] = i;
      if (menuIndex == i)
        gI = mIx;
      mIx++;
    }
  }
  if ((gI + 1) == mIx)
    gI = 0;
  else
    gI++;
  gosterilenMenuId = menuElemanlari[m[gI]].id;
  //}
}
//Alt menü elemanları varsa ilkini görüntülemek için ayarlar
void menuSag()
{
  for (byte i = 0; i < menuAdedi(); i++)
  {
    if (menuElemanlari[i].ustId == gosterilenMenuId)
    {
      gosterilenMenuId = menuElemanlari[i].id;
      return;
    }
  }
}
//Bir üst menü elemanına döndürür
void menuSol()
{
  byte menuIndex = bulMenuIndex(gosterilenMenuId);
  //if (menuElemanlari[menuIndex].ustId != 0)
  gosterilenMenuId = menuElemanlari[menuIndex].ustId;
}


//Basılan tuşa göre yapılacak işlem ayarlanır
void DurumDegerlendir()
{
  byte yeni_buton_durum = YeniButonDurumGetir();
  unsigned long yeniZaman = millis();
  //Herhangi bir tuşa basıldıysa
  if (yeniZaman - eskiZaman > 300 && yeni_buton_durum != 0) //yeni_buton_durum != buton_durum
  {
    eskiZaman = millis();
    buton_durum = yeni_buton_durum;
    zamanKontrol = true;
    lcd.backlight();
    switch (buton_durum)
    {
      case saga://sag
        {
          if (seciliMenuId == 0)
            menuSag();
          else
            DegerDegistir(1);
          EkranOlustur();
          break;
        }
      case sola://sol
        {
          if (seciliMenuId == 0)
            menuSol();
          else
            DegerDegistir(-1);
          EkranOlustur();
          break;
        }
      case yukariya://yukarı
        {
          if (seciliMenuId == 0)
            menuYukari();
          else
            DegerDegistir(10);
          EkranOlustur();
          break;
        }
      case asagiya://asağı
        {
          if (seciliMenuId == 0)
            menuAsagi();
          else
            DegerDegistir(-10);
          EkranOlustur();
          break;
        }
      case tamama://tamam
        {
          if (seciliMenuId == 0 && menuEl(gosterilenMenuId).degerVar)
          {
            seciliMenuId = gosterilenMenuId;
          }
          else if (seciliMenuId != 0)
          {
            byte menuIndex = bulMenuIndex(seciliMenuId);
            if (menuElemanlari[menuIndex].eprom)
              EEPROM.write(seciliMenuId, menuElemanlari[menuIndex].gecerliDeger);
            seciliMenuId = 0;

          }
          EkranOlustur();
          break;
        }
    }
  }
}
//Menü elemanının değeri istenilen değer eklenerek yada çıkartılarak ayarlanır
void DegerDegistir(byte stp)
{
  byte menuIndex = bulMenuIndex(seciliMenuId);
  menuElemanlari[menuIndex].gecerliDeger += stp;
  if (menuElemanlari[menuIndex].gecerliDeger < menuElemanlari[menuIndex].enDusuk)
    menuElemanlari[menuIndex].gecerliDeger = menuElemanlari[menuIndex].enDusuk;
  if (menuElemanlari[menuIndex].gecerliDeger > menuElemanlari[menuIndex].enFazla)
    menuElemanlari[menuIndex].gecerliDeger = menuElemanlari[menuIndex].enFazla;

}
//Hangi butona göre işlem yapılacağı belirlenir
byte YeniButonDurumGetir()
{
  byte yeni_buton_durum = hareketYok;
  if (sag_durum == 1 && sol_durum == 0 && yukari_durum == 0 && asagi_durum == 0 && tamam_durum == 0)
  {
    yeni_buton_durum = saga;
  }
  else if (sag_durum == 0 && sol_durum == 1 && yukari_durum == 0 && asagi_durum == 0 && tamam_durum == 0)
  {
    yeni_buton_durum = sola;
  }
  else if (sag_durum == 0 && sol_durum == 0 && yukari_durum == 1 && asagi_durum == 0 && tamam_durum == 0)
  {
    yeni_buton_durum = yukariya;
  }
  else if (sag_durum == 0 && sol_durum == 0 && yukari_durum == 0 && asagi_durum == 1 && tamam_durum == 0)
  {
    yeni_buton_durum = asagiya;
  }
  else if (sag_durum == 0 && sol_durum == 0 && yukari_durum == 0 && asagi_durum == 0 && tamam_durum == 1)
  {
    yeni_buton_durum = tamama;
  }
  else
  {
    yeni_buton_durum = hareketYok;
  }
  return yeni_buton_durum;
}

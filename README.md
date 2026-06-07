# Kahveci01 (Coffee Shop)

Unreal Engine **5.7** ile geliştirilen kahve dükkanı projesi (C++ tabanlı).

## Gereksinimler

Projeyi açmadan önce şunlar kurulu olmalı:

- **Git** — https://git-scm.com/download/win
- **Unreal Engine 5.7** — Epic Games Launcher üzerinden (tam olarak bu sürüm)
- **Visual Studio 2022** — "Game development with C++" iş yükü seçili olarak

## Projeyi Alma (Clone)

Bir klasör seçip terminalde:

```bash
git clone https://github.com/caninceler/coffe_shop.git
```

## Projeyi Açma

1. İndirilen klasöre girin.
2. `Kahveci01.uproject` dosyasına **sağ tıklayın → "Generate Visual Studio project files"**.
   (C++ projesi olduğu için derleme dosyaları her makinede yeniden üretilir.)
3. `Kahveci01.uproject` dosyasına **çift tıklayın** → Unreal Engine açılır.
4. İlk açılışta "projeyi derle?" sorusuna **Evet** deyin (birkaç dakika sürebilir).

## Çalışmaya Devam Etme

Yeni değişiklikleri çekmek için:

```bash
git pull
```

Değişikliklerinizi göndermek için:

```bash
git add .
git commit -m "açıklama"
git push
```

> Kolaylık için depoda hazır `guncelle.bat` (çek) ve `gonder.bat` (gönder)
> scriptleri de bulunur — çift tıklayarak kullanabilirsiniz.

## Notlar

- `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/` klasörleri otomatik
  üretilir ve git'e dahil **edilmez** (`.gitignore`). Bu normaldir; proje açılınca
  yeniden oluşurlar.

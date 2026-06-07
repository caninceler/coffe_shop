# AGENTS.md — AI Araçları İçin Giriş Noktası

> Bu dosya, bu repoda çalışan **tüm AI araçları** (Codex, Claude Code, Copilot vb.) içindir.
> Codex bu dosyayı otomatik okur. Diğer araçlar da kod yazmadan önce okumalıdır.

## ⚠️ ÖNCE BUNU OKU

Bu projede **herhangi bir kod yazmadan, varlık eklemeden veya öneri yapmadan önce**
[`GAME.md`](GAME.md) dosyasını **baştan sona oku**. O dosya bu projenin tek kaynak
(single source of truth) belgesidir — hem oyun tasarımı (GDD) hem de teknik/mimari
kuralları içerir. Tüm kararlar oradaki kurallara uymak zorundadır.

## Proje Özeti

- **Oyun:** "kahveci01" — first-person, **cozy** kahve dükkanı işletme simülasyonu.
- **Motor:** Unreal Engine 5.7 (C++ modülü + Blueprint hibrit).
- **Çekirdek felsefe:** **Cozy. Zaman sınırı yok, challenge yok, ceza yok.**
  Para sıfırlanmaz, oyun sonu yok. Detaylar `GAME.md` 1.2'de.

## Zorunlu Kurallar (özet — tamamı GAME.md 2.5'te)

1. **Önce oku, sonra yaz.** `GAME.md`'yi ve ilgili mevcut dosyaları/varlıkları incele.
2. **`Coffee_Shop_Big_Pack` içinde ara.** İhtiyacın olan mesh/blueprint büyük ihtimalle
   pakette zaten vardır — sıfırdan üretme.
3. **Cozy çekirdeği boz ma.** Hiçbir mekanik oyuncuyu cezalandırmamalı.
4. **Türetilmiş klasörlere dokunma:** `Binaries/`, `Intermediate/`, `Saved/`,
   `DerivedDataCache/`, `.vs/` — otomatik üretilir, `.gitignore` ile hariç tutulmuştur.
5. **Unreal isimlendirme konvansiyonlarına uy** (GAME.md 2.4).
6. **Binary varlık (`.uasset`/`.umap`) üretme.** AI metin tabanlı çalışır: C++, `.ini`,
   `.md`, `Build.cs`. Binary varlıkları editörde insan oluşturur.
7. **Belirsizse sor.** Karar `GAME.md`'de yoksa ve sonucu önemliyse, varsayım yapma.
8. **Kalıcı kararları `GAME.md`'ye işle** ve alttaki "Son güncelleme" tarihini güncelle.

## Git Akışı (birlikte çalışma)

```bash
git pull              # çalışmaya başlamadan önce daima güncel çek
git add -A
git commit -m "Açıklayıcı mesaj"
git push
```

- `master` ana daldır.
- Push'tan önce daima `git pull` yap (çakışmaları azaltır).
- Büyük binary `.uasset` değişikliklerini commitlerken dikkatli ol; ikili dosyalar
  satır satır birleştirilemez — aynı varlığı aynı anda iki kişi düzenlemekten kaçının.

# KAHVECİ — Oyun Tasarım & AI Geliştirme Dökümanı

> **Bu dosya nedir?**
> Bu, "kahveci01" projesinin tek kaynak (single source of truth) dökümanıdır. Hem **oyun tasarım dökümanı (GDD)** hem de **AI araçları için geliştirme talimatlarını** içerir.
> AI araçları (Claude Code, Copilot vb.) herhangi bir kod yazmadan, varlık eklemeden veya öneri yapmadan **ÖNCE** bu dosyayı okumalıdır. Tüm tasarım ve teknik kararlar buradaki kurallara uymalıdır.
> Oyun veya teknik yapı değiştikçe bu dosya güncel tutulmalıdır.

---

## BÖLÜM 1 — OYUN TASARIM DÖKÜMANI (GDD)

### 1.1 Tek Cümlede Oyun
İlk şahıs (first-person) bakış açısıyla oynanan, oyuncunun kendi kahve dükkanını işlettiği, kahve hazırlayıp müşterilere servis ederek para kazandığı ve bu parayla dükkanını büyüttüğü bir **işletme simülasyonu / tycoon** oyunu.

### 1.2 Vizyon & His (Pillars)
Oyunun her tasarım kararı şu üç sütuna hizmet etmelidir:

1. **"Tezgahın arkasındayım" hissi** — Oyuncu gerçekten bir baristadır. Kahve yapmak elle, adım adım, tatmin edici bir etkileşimdir; menüden seçilen soyut bir işlem değil.
2. **Görünür büyüme** — Kazanılan her para dükkanda fiziksel olarak görülür: yeni makineler, daha fazla masa, dekorasyon, genişleyen mekan. İlerleme gözle takip edilebilir olmalı.
3. **Yönetilebilir kaos** — Yoğunluk arttıkça (daha fazla müşteri, daha karmaşık siparişler) baskı artar; oyuncu sistemleri optimize ederek (personel, ekipman, düzen) bu kaosu yönetir.

### 1.3 Hedef Kitle & Referanslar
- **Hedef kitle:** İşletme/simülasyon oyunu sevenler, cozy + tycoon kesişimi.
- **Tonu:** Sakin ama meşgul; rahatlatıcı ama ödüllendirici. Stresli değil, akış (flow) odaklı.
- **Referanslar (ton ve mekanik için):** Coffee Shop Simulator, Internet Cafe Simulator, PlateUp!, Supermarket Simulator, TCG Card Shop Simulator.
- **NOT:** Bunlar referanstır, kopyalanacak şablon değil. Kimlik 1.2'deki sütunlardır.

### 1.4 Temel Oyun Döngüsü (Core Loop)
```
PREP (Hazırlık)  →  SERVICE (Servis/Yoğunluk)  →  CLOSE (Kapanış)  →  UPGRADE (Yatırım)  →  (tekrar)
```
1. **Prep:** Gün başlamadan stok kontrolü, makine doldurma, menü/fiyat ayarı, dükkan düzeni.
2. **Service:** Müşteriler gelir, sipariş verir; oyuncu first-person olarak kahveyi hazırlar ve servis eder. Hız + doğruluk = bahşiş & memnuniyet.
3. **Close:** Gün sonu özeti — gelir, gider, memnuniyet, kalan stok.
4. **Upgrade:** Kazanılan parayla ekipman, mobilya, dekor, personel veya mekan genişlemesi satın alınır → bir sonraki gün daha kapasiteli başlar.

### 1.5 Temel Mekanikler

| Mekanik | Açıklama | Öncelik |
|---|---|---|
| **First-person etkileşim** | Bak + etkileşim tuşu (E) ile objelerle etkileşim. `BPI_Interact` arayüzü kullanılır. | MVP |
| **Kahve hazırlama** | Adımlı süreç: bardak al → makine/öğütücü kullan → süt/şurup ekle → servis et. | MVP |
| **Sipariş sistemi** | Müşteri belirli bir ürün ister; oyuncu doğru ürünü doğru şekilde hazırlamalı. | MVP |
| **Müşteri & sıra (queue)** | Müşteriler gelir, sıraya girer, sipariş verir, sabır göstergesi azalır. | MVP |
| **Para & ekonomi** | Satıştan gelir, malzeme/fatura gideri, bahşiş. | MVP |
| **Upgrade / satın alma** | Ekipman, mobilya, dekor, mekan. Görünür kurulum. | MVP+1 |
| **Stok yönetimi** | Çekirdek, süt, bardak, şurup tükenir; sipariş edilir. | MVP+1 |
| **Personel (NPC barista/kasiyer)** | İşe alınır, görev atanır, maaş öder. | Sonraki |
| **Memnuniyet / itibar** | Hız & kalite → puan → daha çok müşteri & fiyat toleransı. | Sonraki |
| **Gün/zaman & yoğunluk dalgaları** | Sabah/öğle yoğunluğu gibi ritim. | Sonraki |

> **Kapsam kuralı:** Önce MVP işaretli mekanikler eksiksiz ve eğlenceli çalışmalı. "MVP+1" ve "Sonraki" yalnızca MVP sağlam olduğunda ele alınır. Yeni mekanik önermeden önce mevcut MVP'nin tamam olup olmadığını kontrol et.

### 1.6 Ekonomi Tasarım Hedefleri (denge için pusula)
- Oyuncu her günün sonunda küçük ama anlamlı bir ilerleme hissetmeli.
- İlk büyük upgrade ~2-3 gün içinde erişilebilir olmalı (erken ödül).
- Para enflasyonu yok: ileri seviye upgrade'ler ileri seviye giderleri (kira, maaş, stok) gerektirsin.
- *(Gerçek sayılar `Config` veya bir DataTable'da tutulur — bu dosyada sabit sayı tutma, sadece hedef.)*

### 1.7 Oyuncu Deneyimi Yol Haritası (kabaca)
- **Dakika 0-5:** Tek bir müşteriye tek bir kahve yap. Etkileşimi öğren.
- **Dakika 5-20:** Birden fazla siparişi yönet, ilk parayı kazan, ilk upgrade'i al.
- **Saat 1+:** Stok, menü çeşitliliği, dükkan düzeni optimizasyonu.
- **Uzun vade:** Personel, genişleme, itibar, prestij.

---

## BÖLÜM 2 — TEKNİK TEMEL & AI GELİŞTİRME KURALLARI

### 2.1 Proje Künyesi
- **Motor:** Unreal Engine **5.7**
- **Proje adı / modül:** `kahveci01` (Runtime, C++ modülü mevcut)
- **Birincil platform varsayımı:** PC (Windows). Klavye/fare, first-person kontrol.
- **Aktif plugin:** `ModelingToolsEditorMode` (yalnızca Editor).
- **Kaynak dili:** C++ (çekirdek sistemler) + Blueprint (gameplay glue, UI, etkileşim). Hibrit yaklaşım — aşağıya bak.

### 2.2 Klasör Yapısı (mevcut)
```
kahveci01/
├── GAME.md                  ← BU DOSYA (oku!)
├── kahveci01.uproject
├── Config/                  ← DefaultEngine/Game/Input/Editor.ini
├── Source/kahveci01/        ← C++ modülü (kahveci01.Build.cs, .cpp, .h)
├── Content/
│   └── Coffee_Shop_Big_Pack/  ← Satın alınmış varlık paketi (aşağıya bak)
└── Binaries / Intermediate / Saved / DerivedDataCache  ← türetilmiş, ELLEME
```

### 2.3 Mevcut Varlıklar — `Coffee_Shop_Big_Pack`
Bu pakette gameplay için doğrudan kullanılabilir varlıklar var. **Yeni varlık üretmeden önce bunları kullan:**

- **`Blueprints/`** — `BPI_Interact` (etkileşim arayüzü — etkileşim sisteminin temeli), `E_Key_Widget` (E tuşu prompt widget'ı), `BP_Door_Interior/Exterior`, `BP_BookShelf`, `Ceiling_Lights`.
- **`Maps/`** — `Coffe_Shop_Demo_Map`, `Coffee_Shop_Overview` (mekan/sahne temeli).
- **`Meshes/`** — `Counter` (tezgah), `Coffee_Equipment` (kahve makineleri), `Cups` & `Paper_Cups` (bardaklar), `POS` (kasa), `Syrup` (şuruplar), `Jar`, `Furniture`, `Chalkboard`, `Wall_Menu`, `LED_Sign`, `Doors`, `Walls_and_Windows`, `Restroom` vb.
- **`Materials/ Particles/ Sound/ Textures/`** — destek varlıkları.

> **Varlık kuralı:** İhtiyaç duyulan mesh/blueprint büyük ihtimalle pakette zaten vardır. Önce `Coffee_Shop_Big_Pack` içinde ara. Sadece pakette karşılığı yoksa yeni varlık öner.

### 2.4 Kod & Blueprint Mimari Kuralları

**C++ vs Blueprint ayrımı:**
- **C++:** Çekirdek sistemler ve veri — ekonomi, sipariş/kahve state machine, envanter/stok, kayıt/yükleme, gameplay-critical sınıflar (`GameMode`, `GameState`, `PlayerController`, ana `Actor`/`Component`'ler). Performans ve veri bütünlüğü gereken her şey.
- **Blueprint:** Görsel/etkileşim glue, UI (UMG), animasyon, ses tetikleme, seviye-spesifik mantık, `BPI_Interact` implementasyonları. Hızlı iterasyon gereken her şey.
- **Köprü:** C++ sınıflarını `UCLASS(Blueprintable, BlueprintType)`, fonksiyonları `UFUNCTION(BlueprintCallable)`, değişkenleri `UPROPERTY(EditAnywhere/BlueprintReadWrite)` ile Blueprint'e aç. Tasarımcının dokunacağı her sayı `UPROPERTY` ile editöre çıkmalı.

**İsimlendirme (Unreal konvansiyonu — ZORUNLU):**
- C++ Actor sınıfları: `A` öneki (`AKahveciPlayerCharacter`, `ACoffeeMachine`).
- C++ UObject/Component: `U` öneki (`UInventoryComponent`, `UOrderComponent`).
- C++ struct: `F`, enum: `E`, interface: `I` / `U`.
- Blueprint varlıkları: `BP_` (Blueprint), `BPI_` (Interface), `WBP_` veya `W_` (Widget), `BPC_` (Component), `DT_` (DataTable), `DA_` (DataAsset).
- Bool değişkenler: `b` öneki (`bIsOrderComplete`).
- Proje sınıfları için tutarlı bir tema öneki kullan (örn. `Kahveci` veya `CoffeeShop`) — karışık kullanma, birini seçip tut.

**Genel kurallar:**
- Sabit sayıları (fiyat, süre, kapasite) **koda gömme**. `DataTable` / `DataAsset` / `UPROPERTY` ile dışarı al, tasarımcı düzenleyebilsin.
- Sihirli string yerine `enum` / `FName` sabitleri kullan.
- Tek sorumluluk: bir Actor çok şey yapmasın; Component'lere böl (`UInventoryComponent`, `UInteractableComponent`).
- Etkileşim sistemi mevcut `BPI_Interact` arayüzü üzerine kurulmalı — paralel ikinci bir sistem icat etme.

### 2.5 AI Araçları İçin Çalışma Kuralları (ZORUNLU)
Herhangi bir AI aracı bu projede çalışırken:

1. **Önce oku, sonra yaz.** Değişiklikten önce ilgili mevcut dosyaları/varlıkları incele. `Coffee_Shop_Big_Pack` içinde karşılığı olan bir şeyi sıfırdan üretme.
2. **Sütunlara sadık kal (1.2).** Önerilen her özellik üç tasarım sütunundan en az birine hizmet etmeli. Etmiyorsa önerme.
3. **Kapsamı koru (1.5).** MVP tamamlanmadan "Sonraki" özelliklere geçme. Scope creep'i aktif olarak reddet.
4. **Türetilmiş klasörlere dokunma.** `Binaries`, `Intermediate`, `Saved`, `DerivedDataCache`, `.vs` — bunlar otomatik üretilir, elle düzenleme yok.
5. **Unreal konvansiyonlarına uy (2.4).** İsimlendirme ve C++/BP ayrımı zorunludur.
6. **Binary varlık üretme.** AI metin tabanlı çalışır: C++ kodu, `.ini` config, `.md` döküman, Build.cs düzenle. `.uasset`/`.umap` binary dosyalarını editörde insan oluşturur — AI bunları bozmaz, sadece nasıl kurulacağını tarif eder.
7. **Belirsizse sor.** Bir tasarım kararı bu dosyada yoksa ve sonucu önemliyse, varsayım yapıp ilerlemeden önce netleştir.
8. **Bu dosyayı güncel tut.** Kalıcı bir tasarım/mimari kararı verildiğinde ilgili bölümü güncelle.

### 2.6 İlk Geliştirme Adımları (öneri — MVP yolu)
Bu sıra, oynanabilir bir dikey dilim (vertical slice) hedefler:
1. First-person `Pawn`/`Character` + kontrolcü (hareket + bakış + E ile etkileşim).
2. `BPI_Interact` tabanlı etkileşim sistemi (objeye bak → prompt → E → aksiyon).
3. Tek bir kahve hazırlama akışı (bardak al → makine → servis et).
4. Tek müşteri + tek sipariş + para kazanma.
5. Gün sonu özeti + tek bir upgrade satın alma.
> Bu 5 adım çalışınca MVP iskeleti hazırdır; sonra 1.5'teki MVP listesini genişlet.

---

## BÖLÜM 3 — AÇIK SORULAR / KARAR BEKLEYENLER
*(AI veya tasarımcı buraya kararsız konuları yazar; çözülünce ilgili bölüme taşınır.)*

- [ ] Sanatsal stil net mi? (gerçekçi / stilize / low-poly) — paket stiline bak.
- [ ] Hedef oturum uzunluğu (1 gün = kaç dakika gerçek zaman)?
- [ ] Personel sistemi MVP+1 mi yoksa daha sonra mı?
- [ ] Kayıt/yükleme (save) MVP'ye dahil mi?

---
*Son güncelleme: 2026-06-05 — kuruluş. Bu dosya değiştikçe tarih ve ilgili bölümü güncelleyin.*

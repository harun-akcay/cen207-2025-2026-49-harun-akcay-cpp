# 🔧 Kurulum Adımları (Admin Yetkisi Gerekli)

## ⚠️ ÖNEMLİ: PowerShell'i Admin Olarak Açın

1. **Windows tuşuna basın**
2. **"PowerShell" yazın**
3. **Sağ tıklayın → "Run as administrator" seçin**
4. **"Yes" butonuna tıklayın**

## 📝 Kurulum Komutları

### Adım 1: Proje Klasörüne Gidin
```powershell
cd "C:\Users\zumre\OneDrive\Desktop\Proje\cen207-2025-2026-49-harun-akcay-cpp"
```

### Adım 2: Install Script'ini Çalıştırın
```powershell
.\4-install-windows-enviroment.bat
```

Bu script otomatik olarak şunları kuracak:
- ✅ Chocolatey (paket yöneticisi)
- ✅ Doxygen (dokümantasyon)
- ✅ Python + pip (Python paketleri için)
- ✅ OpenCppCoverage (test coverage)
- ✅ coverxygen (dokümantasyon coverage)
- ✅ junit2html (test HTML)
- ✅ Diğer gerekli araçlar

### Adım 3: Kurulum Tamamlandıktan Sonra

Build script'ini tekrar çalıştırın:
```powershell
.\7-build-app-windows.bat
```

---

## 📊 Kurulacak Araçlar

| Araç | Ne İçin? | Nasıl Kurulur? |
|------|----------|----------------|
| **Chocolatey** | Paket yöneticisi | Admin PowerShell ile otomatik |
| **Doxygen** | Documentation | `choco install doxygen.install -y` |
| **Python** | Python paketleri | `choco install python -y` |
| **OpenCppCoverage** | Test Coverage | `choco install opencppcoverage -y` |
| **coverxygen** | Doc Coverage | `pip install coverxygen` |
| **junit2html** | Test HTML | `pip install junit2html` |

---

## ✅ Kurulum Sonrası Kontrol

Kurulum tamamlandıktan sonra şu komutlarla kontrol edin:

```powershell
doxygen --version
python --version
pip --version
choco --version
```

---

**Not:** Admin yetkisi olmadan bu araçlar kurulamaz. PowerShell'i mutlaka Admin olarak açın!



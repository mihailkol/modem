#pragma once
// Две "встроенные" вкладки, не привязанные к конкретному device/module:
//   - Мониторинг: живые значения всех каналов Bus (/api/channels), с
//     возможностью задать уставки насосов (/api/channels/set).
//   - Настройки: сеть/время/бэкап/OTA — перенос старого main.cpp'шного
//     SETTINGS_HTML из esp32-platform, только вынесено из main.cpp сюда,
//     чтобы не раздувать main.cpp обратно (см. ТЗ, п.1 — жалоба на старый
//     main.cpp как раз про инлайн-HTML вперемешку с #ifdef).
//
// Порядок регистрации важен для порядка вкладок в nav (см. main.cpp):
// Мониторинг регистрируется первым (это дефолтная активная вкладка),
// Настройки — последней. Диагностика — сырые ADC/DI/DO/raw.*-каналы без
// калибровок (см. BuiltinTabs.cpp), скрыта из nav по умолчанию тумблером
// BaseConfig::show_diagnostics — регистрируется где-то между ними.
namespace BuiltinTabs {
    void registerMonitorTab();
    void registerDiagTab();
    void registerSettingsTab();
}

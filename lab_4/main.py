import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import butter, filtfilt

# 1. Функція фільтрації (Баттерворт)
def butter_lowpass_filter(data, cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    # Обмеження, щоб частота зрізу не перевищувала частоту Найквіста
    if normal_cutoff >= 1: normal_cutoff = 0.99
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return filtfilt(b, a, data)

# 2. Функція зашумленої гармоніки (згідно з технічним завданням)
def harmonic_with_noise(t, amplitude, frequency, phase, noise_mean, noise_covariance, show_noise, noise_array):
    harmonic = amplitude * np.sin(2 * np.pi * frequency * t + phase)
    if show_noise:
        return harmonic + noise_array
    return harmonic

# Початкові значення параметрів
INIT_AMP = 1.0
INIT_FREQ = 0.5
INIT_PHASE = 0.0
INIT_MEAN = 0.0
INIT_COV = 0.1
INIT_CUTOFF = 5.0
FS = 100  # Частота дискретизації
t = np.linspace(0, 10, 1000)

# Глобальний масив шуму (щоб він не змінювався при зміні параметрів гармоніки)
current_noise = np.random.normal(INIT_MEAN, np.sqrt(INIT_COV), len(t))

# Налаштування графічного інтерфейсу
fig, ax = plt.subplots(figsize=(10, 7))
plt.subplots_adjust(bottom=0.35, left=0.15)

# Початкові обчислення сигналів
y_pure = INIT_AMP * np.sin(2 * np.pi * INIT_FREQ * t + INIT_PHASE)
y_noisy = harmonic_with_noise(t, INIT_AMP, INIT_FREQ, INIT_PHASE, INIT_MEAN, INIT_COV, True, current_noise)
y_filt = butter_lowpass_filter(y_noisy, INIT_CUTOFF, FS)

# Відображення ліній на графіку
line_noisy, = ax.plot(t, y_noisy, color='orange', label='Зашумлений', alpha=0.7)
line_pure, = ax.plot(t, y_pure, color='blue', linestyle='--', label='Чиста гармоніка')
line_filt, = ax.plot(t, y_filt, color='purple', label='Відфільтрований', linewidth=2)

ax.set_ylim(-3, 3)
ax.legend(loc='upper right')
ax.grid(True, linestyle=':')

# Створення осей для слайдерів (вертикальне розташування)
ax_amp = plt.axes([0.2, 0.28, 0.5, 0.03])
ax_freq = plt.axes([0.2, 0.24, 0.5, 0.03])
ax_phase = plt.axes([0.2, 0.20, 0.5, 0.03]) 
ax_mean = plt.axes([0.2, 0.13, 0.5, 0.03])
ax_cov = plt.axes([0.2, 0.09, 0.5, 0.03])
ax_cut = plt.axes([0.2, 0.05, 0.5, 0.03])

# Ініціалізація слайдерів
s_amp = Slider(ax_amp, 'Amplitude', 0.1, 2.0, valinit=INIT_AMP)
s_freq = Slider(ax_freq, 'Frequency', 0.1, 2.0, valinit=INIT_FREQ)
s_phase = Slider(ax_phase, 'Phase', 0.0, 2*np.pi, valinit=INIT_PHASE)
s_mean = Slider(ax_mean, 'Noise Mean', -1.0, 1.0, valinit=INIT_MEAN)
s_cov = Slider(ax_cov, 'Noise Cov', 0.0, 1.0, valinit=INIT_COV)
s_cut = Slider(ax_cut, 'Filter Cutoff', 0.1, 20.0, valinit=INIT_CUTOFF)

# Налаштування кнопки Reset та чекбокса
ax_res = plt.axes([0.8, 0.02, 0.1, 0.04])
btn_res = Button(ax_res, 'Reset')
ax_check = plt.axes([0.02, 0.45, 0.1, 0.1], frameon=False)
check = CheckButtons(ax_check, ['Show Noise'], [True])

# Функція оновлення графіків при зміні параметрів
def update(val):
    global current_noise
    
    # Визначаємо, чи потрібно перегенерувати шум (тільки якщо змінено параметри шуму)
    # Перевіряємо, чи викликана подія саме слайдерами шуму
    if val in [s_mean.val, s_cov.val]:
        current_noise = np.random.normal(s_mean.val, np.sqrt(s_cov.val), len(t))
    
    # Отримуємо стан чекбокса та значення слайдерів
    show_n = check.get_status()[0]
    amp = s_amp.val
    freq = s_freq.val
    phase = s_phase.val
    cutoff = s_cut.val
    
    # Оновлюємо дані сигналів
    y_p = amp * np.sin(2 * np.pi * freq * t + phase)
    y_n = harmonic_with_noise(t, amp, freq, phase, s_mean.val, s_cov.val, show_n, current_noise)
    
    # Для фільтрації завжди використовуємо актуальний зашумлений сигнал
    y_n_for_filter = y_p + current_noise
    y_f = butter_lowpass_filter(y_n_for_filter, cutoff, FS)
    
    # Оновлюємо графічні об'єкти
    line_pure.set_ydata(y_p)
    line_noisy.set_ydata(y_n)
    line_filt.set_ydata(y_f)
    
    # Перемальовуємо фігуру
    fig.canvas.draw_idle()

# Прив'язка подій
s_amp.on_changed(update)
s_freq.on_changed(update)
s_phase.on_changed(update)
s_mean.on_changed(update)
s_cov.on_changed(update)
s_cut.on_changed(update)
check.on_clicked(update)

# Функція скидання параметрів
def reset(event):
    s_amp.reset()
    s_freq.reset()
    s_phase.reset()
    s_mean.reset()
    s_cov.reset()
    s_cut.reset()
    # Оновлюємо шум до початкового стану
    update(None)

btn_res.on_clicked(reset)

plt.show()
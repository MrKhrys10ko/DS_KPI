import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Налаштування сторінки
st.set_page_config(page_title="Data Science Lab 5", layout="wide")

# Функція для завантаження даних
@st.cache_data
def load_data():
    df = pd.read_csv('vhi_data.csv')
    return df

df = load_data()

# Словник для областей
region_dict = {
    1: "Вінницька", 2: "Волинська", 3: "Дніпропетровська", 4: "Донецька", 
    5: "Житомирська", 6: "Закарпатська", 7: "Запорізька", 8: "Івано-Франківська",
    9: "Київська", 10: "Кіровоградська", 11: "Луганська", 12: "Львівська",
    13: "Миколаївська", 14: "Одеська", 15: "Полтавська", 16: "Рівненська",
    17: "Сумська", 18: "Тернопільська", 19: "Харківська", 20: "Херсонська",
    21: "Хмельницька", 22: "Черкаська", 23: "Чернівецька", 24: "Чернігівська", 25: "Крим"
}

# Заголовок
st.title("Аналіз індексів VCI, TCI, VHI для областей України")

# Бічна панель
st.sidebar.header("Параметри фільтрації")

# Кнопка скидання 
if 'reset' not in st.session_state:
    st.session_state.reset = False

def reset_filters():
    st.session_state.selected_index = "VHI"
    st.session_state.selected_region = 1
    st.session_state.weeks = (1, 52)
    st.session_state.years = (int(df['Year'].min()), int(df['Year'].max()))
    st.session_state.sort_asc = False
    st.session_state.sort_desc = False

if st.sidebar.button("Скинути всі фільтри"):
    reset_filters()

# Елементи керування
selected_index = st.sidebar.selectbox("Оберіть індекс:", ["VCI", "TCI", "VHI"], key='selected_index')
selected_region = st.sidebar.selectbox("Оберіть область:", options=list(region_dict.keys()), 
                                       format_func=lambda x: region_dict[x], key='selected_region')

weeks_range = st.sidebar.slider("Інтервал тижнів:", 1, 52, (1, 52), key='weeks')
years_range = st.sidebar.slider("Інтервал років:", int(df['Year'].min()), int(df['Year'].max()), 
                                (int(df['Year'].min()), int(df['Year'].max())), key='years')

st.sidebar.markdown("---")
sort_asc = st.sidebar.checkbox("Сортувати за зростанням", key='sort_asc')
sort_desc = st.sidebar.checkbox("Сортувати за спаданням", key='sort_desc')


if sort_asc and sort_desc:
    st.sidebar.warning("Обрано обидва типи сортування. Буде використано сортування за зростанням.")

# Фільтрування даних
filtered_df = df[
    (df['Region'] == selected_region) &
    (df['Year'] >= years_range[0]) & (df['Year'] <= years_range[1]) &
    (df['Week'] >= weeks_range[0]) & (df['Week'] <= weeks_range[1])
]

# Сортування
if sort_asc:
    filtered_df = filtered_df.sort_values(by=selected_index, ascending=True)
elif sort_desc:
    filtered_df = filtered_df.sort_values(by=selected_index, ascending=False)

# Вкладки
tab1, tab2, tab3 = st.tabs(["Таблиця даних", "Часовий ряд", "Порівняння областей"])

with tab1:
    st.subheader(f"Дані для {region_dict[selected_region]}")
    st.dataframe(filtered_df, use_container_width=True)

with tab2:
    st.subheader(f"Графік часового ряду індексу {selected_index}")
    if not filtered_df.empty:
        fig, ax = plt.subplots(figsize=(10, 5))
        # Створюємо часову вісь для графіка
        filtered_df['Date'] = filtered_df['Year'].astype(str) + "-W" + filtered_df['Week'].astype(str)
        
        sns.lineplot(data=filtered_df, x='Date', y=selected_index, ax=ax, marker='o')
        plt.xticks(rotation=45)
        # Показуємо тільки кожен 10-й підпис, щоб не перевантажувати графік
        for i, label in enumerate(ax.get_xticklabels()):
            if i % 10 != 0: label.set_visible(False)
        st.pyplot(fig)
    else:
        st.write("Дані для відображення відсутні.")

with tab3:
    st.subheader(f"Порівняння {selected_index} між областями")
    
    # Дані для всіх областей за той самий період
    comp_df = df[
        (df['Year'] >= years_range[0]) & (df['Year'] <= years_range[1]) &
        (df['Week'] >= weeks_range[0]) & (df['Week'] <= weeks_range[1])
    ]
    
    if not comp_df.empty:
        fig2, ax2 = plt.subplots(figsize=(12, 6))
        
        # Малюємо всі області сірим кольором 
        sns.lineplot(data=comp_df, x='Year', y=selected_index, hue='Region', 
                     palette=['lightgray']*len(df['Region'].unique()), legend=False, alpha=0.5, ax=ax2)
        
        # Виділяємо обрану область яскравим кольором
        region_data = comp_df[comp_df['Region'] == selected_region]
        sns.lineplot(data=region_data, x='Year', y=selected_index, color='red', linewidth=2.5, 
                     label=region_dict[selected_region], ax=ax2)
        
        plt.title(f"Порівняння {selected_index} ({years_range[0]}-{years_range[1]})")
        st.pyplot(fig2)
    else:
        st.write("Немає даних для порівняння.")
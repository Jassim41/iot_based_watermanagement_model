import streamlit as st
import matplotlib.pyplot as plt
import numpy as np
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

# Fetch the service account key JSON file contents
cred = credentials.Certificate('D:\downloads\sample-b8bc5-firebase-adminsdk-bwc4x-c9a7addc79.json')
rainfall_value = None
soil_moisture_value = None
temp_value = None
# Check if the app is already initialized
if not firebase_admin._apps:
    # Initialize the app with a service account, granting admin privileges
    firebase_admin.initialize_app(cred, {
        'databaseURL': 'https://sample-b8bc5-default-rtdb.firebaseio.com'  # Update with your database URL
    })

# Reference to the "SensorData" path
sensor_data_ref = db.reference('SensorData')

# Retrieve the latest sensor data
latest_data = sensor_data_ref.order_by_key().limit_to_last(1).get()

def plot_gauge(values, title, needle_value):
    colors = ["#ee4d55", "#f36d54","#fabd57", "#f6ee54", "#c1da64"]

    num_bars = 5
    bar_width = 3.14159 / num_bars  # dividing the circle by the number of bars

    x_axis_vals = [i * bar_width for i in range(num_bars)]

    fig = plt.figure(figsize=(10, 10))
    ax = fig.add_subplot(projection="polar")

    ax.bar(x=x_axis_vals, width=bar_width, height=0.5, bottom=2,
           linewidth=3, edgecolor="white",
           color=colors, align="edge")

    plt.annotate("Very High", xy=(0.16, 2.1), rotation=-75, color="white", fontweight="bold")
    plt.annotate("High", xy=(0.85, 2.08), rotation=-45, color="white", fontweight="bold")
    plt.annotate("Moderate", xy=(1.6, 2.2), rotation=0, color="white", fontweight="bold")
    plt.annotate("Low", xy=(2.2, 2.25), rotation=40, color="white", fontweight="bold")
    plt.annotate("Very Low", xy=(2.95, 2.25), rotation=75, color="white", fontweight="bold")

    # Adjusting value annotations
    locs = [0, 0.60, 1.20, 1.81, 2.4, 3]
    ha_values = ["right" if val <= 20 else "left" for val in values]

    for loc, val, ha in zip(locs, values, ha_values):
        plt.annotate(val, xy=(loc * 3.14159 / 3, 2.5), ha=ha)

    # Calculating angle for arrow based on value
    arrow_value = needle_value   # Example value for arrow position
    arrow_angle = np.interp(arrow_value, [min(values), max(values)], [180, 0])

    # Adjusting arrow position
    plt.annotate(str(arrow_value), xytext=(0, 0), xy=(arrow_angle * np.pi / 180, 2.0),
                 arrowprops=dict(arrowstyle="wedge, tail_width=0.5", color="black", shrinkA=0),
                 bbox=dict(boxstyle="circle", facecolor="black", linewidth=2.0),
                 fontsize=45, color="white", ha="center"
                )

    plt.title(title, loc="center", pad=20, fontsize=35, fontweight="bold")
    ax.set_axis_off()

    return fig

def dashboard():
    
    st.title("Dashboard")
    st.write("This is the Dashboard page.")
    # Display the latest sensor data
    if latest_data:
        latest_entry_key = list(latest_data.keys())[0]
        latest_entry = latest_data[latest_entry_key]
        
        # Extract values and store in variables
        rainfall_value = latest_entry["RainPercentage"]
        soil_moisture_value = latest_entry["SoilMoisture"]
        temp_value = latest_entry["TemperatureC"]
        
        # Display the values
        st.write(f'Rain Percentage: {rainfall_value}')
        st.write(f'Soil Moisture: {soil_moisture_value}')
        st.write(f'Temperature (Celsius): {temp_value}')
    else:
        st.write('No data available.')

    # First row with two gauges
    col1, col2 = st.columns(2)

    with col1:
        st.write("## Temperature Gauge")
        temp_fig = plot_gauge([100, 80, 60, 40, 20, 0], "Temperature", temp_value)
        st.pyplot(temp_fig)

    with col2:
        st.write("## Humidity Gauge")
        humidity_value = st.slider("Humidity", 0, 100, 0)
        humidity_fig = plot_gauge([100, 80, 60, 40, 20, 0], "Humidity", humidity_value)
        st.pyplot(humidity_fig)

    # Second row with two gauges
    col3, col4 = st.columns(2)

    with col3:
        st.write("## Soil Moisture Gauge")
        soil_moisture_fig = plot_gauge([100, 80, 60, 40, 20, 0], "Soil Moisture", soil_moisture_value)
        st.pyplot(soil_moisture_fig)

    with col4:
        st.write("## Rainfall Gauge")
        rainfall_fig = plot_gauge([100, 80, 60, 40, 20, 0], "Rainfall", rainfall_value)
        st.pyplot(rainfall_fig)



def main():
    dashboard()
    
if __name__ == "__main__":
    main()

# CSS styling
st.markdown(
    """
    <style>
    .st-emotion-cache-6qob1r.eczjsme3 {
        background: #F5F5DC; /* Gradient from dark blue to white for the sidebar */
        color: white;
    }

    .st-emotion-cache-1r4qj8v {
        background: linear-gradient(45deg, #008000, #FFFF99); /* Gradient from green to blue for the main area */
        color: white;
    }

    .sidebar-content {
        background-color: #1F618D; /* Dark blue color for the sidebar */
        color: black;
    }

    .title-text {
        color: #2ECC71; /* Deep green color for the title */
    }

    .eyeqlp51.st-emotion-cache-1pbsqtx.ex0cdmw0 {
        color: black; /* Change color to black */
    }

    .st-emotion-cache-18ni7ap.ezrtsby2 {
        display: none; /* Hide the header area */
    }

  
    </style>
    """,
    unsafe_allow_html=True
)


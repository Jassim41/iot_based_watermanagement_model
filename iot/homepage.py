import streamlit as st

st.set_page_config(
    page_title="Multipage App",
    page_icon="👋",
)

st.title("Main Page")
st.sidebar.success("Select a page above.")
def main():    
    st.write("This is the landing page of the application.")
    
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
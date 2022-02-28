from scipy.io import arff
from plotly.subplots import make_subplots
import plotly.graph_objects as go

data = arff.loadarff(r'/home/sara/apre/tpc-1/breast.w.arff')

b_y1, b_y2, b_y3, b_y4, b_y5, b_y6, b_y7, b_y8, b_y9 = ([] for i in range(9))
m_y1, m_y2, m_y3, m_y4, m_y5, m_y6, m_y7, m_y8, m_y9 = ([] for i in range(9))

for aux in data[0]:
    if(aux['Class'].decode('UTF-8') == 'benign'):
        b_y1.append(aux['Clump_Thickness'])
        b_y2.append(aux['Cell_Size_Uniformity'])
        b_y3.append(aux['Cell_Shape_Uniformity'])
        b_y4.append(aux['Marginal_Adhesion'])
        b_y5.append(aux['Single_Epi_Cell_Size'])
        b_y6.append(aux['Bare_Nuclei'])
        b_y7.append(aux['Bland_Chromatin'])
        b_y8.append(aux['Normal_Nucleoli'])
        b_y9.append(aux['Mitoses'])
    else: 
        m_y1.append(aux['Clump_Thickness'])
        m_y2.append(aux['Cell_Size_Uniformity'])
        m_y3.append(aux['Cell_Shape_Uniformity'])
        m_y4.append(aux['Marginal_Adhesion'])
        m_y5.append(aux['Single_Epi_Cell_Size'])
        m_y6.append(aux['Bare_Nuclei'])
        m_y7.append(aux['Bland_Chromatin'])
        m_y8.append(aux['Normal_Nucleoli'])
        m_y9.append(aux['Mitoses'])

# benign class
fig = make_subplots(rows=3, cols=3)

trace1 = go.Histogram(x=b_y1, name="Clump Thickness")
trace2 = go.Histogram(x=b_y2, name="Cell Size Uniformity")
trace3 = go.Histogram(x=b_y3, name="Cell Shape Uniformity")
trace4 = go.Histogram(x=b_y4, name="Marginal Adhesion")
trace5 = go.Histogram(x=b_y5, name="Single Epi Cell Size")
trace6 = go.Histogram(x=b_y6, name="Bare Nuclei")
trace7 = go.Histogram(x=b_y7, name="Bland Chromatin")
trace8 = go.Histogram(x=b_y8, name="Normal Nucleoli")
trace9 = go.Histogram(x=b_y9, name="Mitoses")

fig.append_trace(trace1, 1, 1)
fig.append_trace(trace2, 1, 2)
fig.append_trace(trace3, 1, 3)
fig.append_trace(trace4, 2, 1)
fig.append_trace(trace5, 2, 2)
fig.append_trace(trace6, 2, 3)
fig.append_trace(trace7, 3, 1)
fig.append_trace(trace8, 3, 2)
fig.append_trace(trace9, 3, 3)

fig.update_layout(title_text='Distribution per variable for Benign class', title_x=0.5)
fig.show()

# malign class
fig = make_subplots(rows=3, cols=3)

trace1 = go.Histogram(x=m_y1, name="Clump Thickness")
trace2 = go.Histogram(x=m_y2, name="Cell Size Uniformity")
trace3 = go.Histogram(x=m_y3, name="Cell Shape Uniformity")
trace4 = go.Histogram(x=m_y4, name="Marginal Adhesion")
trace5 = go.Histogram(x=m_y5, name="Single Epi Cell Size")
trace6 = go.Histogram(x=m_y6, name="Bare Nuclei")
trace7 = go.Histogram(x=m_y7, name="Bland Chromatin")
trace8 = go.Histogram(x=m_y8, name="Normal Nucleoli")
trace9 = go.Histogram(x=m_y9, name="Mitoses")

fig.append_trace(trace1, 1, 1)
fig.append_trace(trace2, 1, 2)
fig.append_trace(trace3, 1, 3)
fig.append_trace(trace4, 2, 1)
fig.append_trace(trace5, 2, 2)
fig.append_trace(trace6, 2, 3)
fig.append_trace(trace7, 3, 1)
fig.append_trace(trace8, 3, 2)
fig.append_trace(trace9, 3, 3)

fig.update_layout(title_text='Distribution per variable for Malign class', title_x=0.5)
fig.show()
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import torch
import torchvision
import torchvision.transforms as transforms
from   torch.autograd import Variable
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from   torch.utils.data import Dataset, DataLoader


    
class MyDataset(Dataset):
    def __init__(self, datatable, transform=None):
        self.data  = np.reshape(datatable[:,0:-1],(-1,40)).astype('float32')
        self.label = datatable[:,-1].astype('int')
        self.transform = transform
    
    def __len__(self):
        return len(self.label)
    
    def __getitem__(self, idx):
        sample = {'feature': self.data[idx,:], 'label': self.label[idx]}
        if self.transform:
            sample = self.transform(sample)
        return sample

class Net(nn.Module):
    def __init__(self,n,m1,m2,c):
        super(Net, self).__init__()
        self.fc1 = nn.Linear(n, m1)
        self.fc2 = nn.Linear(m1, m2)
        self.fc3 = nn.Linear(m2, c)

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x
    
 

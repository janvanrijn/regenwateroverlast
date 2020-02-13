import sys
import os
sys.path.append(os.path.realpath('./auto_encoder/'))
sys.path.append(os.path.realpath('./experiments/'))
sys.path.append(os.path.realpath('./auto_encoder/'))
sys.path.append(os.path.realpath('./map_visualisation/'))
sys.path.append(os.path.realpath('./pandafy_data/'))


def start_here():
    '''
        This function serves as a tutorial to get a hang of the method 
        implemented. 
    '''
    #Welcome my dear successor!
    #
    #The last 2.5 months I have put a lot of effort in the project you now have 
    #before you. I think the project has great potential and hope you enjoy 
    #working on it, just as much as I have. I really hope you can take it to the 
    #next level.
    
    #The aim of this project was to combine data of three different sources and 
    #to build a proof-of-concept model. The three different sources are:
    #1. A height map of the Netherlands (AHN2_5m) (INPUT)
    #   source: https://geodata.nationaalgeoregister.nl/ahn2/atom/ahn2_5m.xml
    #2. Precipitation data from the KNMI (rad_nl25_rac_mfbs_01h) (INPUT)
    #   source: https://data.knmi.nl/datasets/rad_nl25_rac_mfbs_01h/2.0
    #3. Tweets with geo location (OUTPUT)
    #   source: Twitter
    
    #In order to combine the data, the raw data is converted into pandas 
    #dataframes and then saved to .csv files. These .csv files can be easily 
    #reloaded into pandas dataframes, which allow for easy data selection and 
    #recombination.
    #Building the proof-of-concept model consist of 4 steps:
    #1. Train an autoencoder on the heightmap
    #2. Generate terrain features using the autoencoder
    #3. Combine different data sets
    #4. Train a Random Forest Classifier on the combined data sets
    
    #These 4 steps can be repeated in a loop to perform automated machine
    #learning. Step 2 and 3 can be swapped if it is desired to generate
    #features on every entry in the combined dataset. The current order of
    #steps is due to time management. Since features are generated per (KNMI) 
    #radar pixel, it saves time to first generate features for every radar 
    #pixel, and then recombine the output with the different datasets.
    
    #Now to get into the details:
    
    #---------------------------------------------------------------------------
    #Getting the data
    #---------------------------------------------------------------------------
    
    #First of all, we need to get the raw data.
    #The AHN2_5m heightmap can be downloaded from 
    #https://geodata.nationaalgeoregister.nl/ahn2/atom/ahn2_5m.xml
    #Since this is an .xml file and not a link to a download, I recommend
    #using the two bash scripts "scrape" and "download". They can be found
    #in this repository in the "scripts" folder. 
    #To download, follow these steps:
    #1. Make a folder called "AHN2_5m" in the superfolder where you downloaded 
    #   this repository. Don't download the data into this repository, since it
    #   is too large to upload to GitHub.
    #2. Copy the "scrape" and "download" scripts to this newly created "AHN2_5m"
    #   folder. Open a Unix terminal, navigate to this "AHN2_5m" folder and
    #   execute the following commands:
    #   ./scrape
    #   ./download
    #3. After the download is complete, unzip all the downloaded .zip files
    #   and then remove the .zip files, to save on disk space.

    #The Precipitation data from the KNMI can be downloaded from: 
    #https://data.knmi.nl/datasets/rad_nl25_rac_mfbs_01h/2.0
    #Copy this link into your browser and click the "Download the matching data"
    #button. Save the .zip file to the superfolder of the repository, unzip it
    #and unzip every .zip file in it.

    #Twitter data can not be downloaded simply, you need a paid subscription to 
    #Twitter developer. In order to get twitter data, follow these steps:
    #1. Log into your Twitter account
    #2. Navigate to https://developer.twitter.com/
    #3. Navigate to Products -> Products overview
    #4. Choose the product you want to purchase. I recommend Premium 
    #   Full-archive 
    #5. Make a new folder "Twitter_cred" in the superfolder of this repository.
    #   This folder will store your Twitter query results. It
    #   is important not to store these query results in the
    #   repository, since in that case they can be viewed by anyone.
    #6. Make a file called "~/.twitter_keys.yaml". Be sure to make this in your
    #   home directory and not in the repository. For more info on how to do
    #   this, see:
    #   https://github.com/twitterdev/search-tweets-python/blob/master/README.rst
    #7. Run the query, or skip this step if you allready have the Tweets:
    skip_twitter_query = True
    if not skip_twitter_query:
        from Twitter.searchtweets_query import searchtweets_query
        searchtweets_query(file_name='../Twitter_cred/full_arch_2007-2020.txt')
    
    #---------------------------------------------------------------------------
    #Training the autoencoder
    #---------------------------------------------------------------------------
    
    #Trainig the autoencoder is done through the class "class_experiment".
    #This class expects a dictionary called "parameters", which defines several
    #parameters of the experiment. The class "class_experiment" allows for
    #easy runs and loads of the experiment. The parameters can be stored in a
    #script as can be seen in the folder "experiments". This way an experiment
    #can be reviewed at later times, while all it's parameters are stored. It
    #can be easily reran by typing "experiment_name.py run" and it's results
    #can be loaded by typing "experiment_name.py load".
    #Below is an example of how to set up an experiment.
        
    parameters = {}
    parameters['experiment_name'] = 'tutorial_experiment'#This name must be unique, since it is used to store the results
    parameters['experiment_type'] = 'varying_pooling_factor'#This string defines the type of experiment, see "experiments/class_experiment.py" for all implemented types
    parameters['encoder_type'] = 'shallow_narrow_cnn'#This stirng defines the type of autoencoder that is built, see "auto_encoder/auto_encoder_train.py" for all implemented types
    parameters['epochs'] = 10#The amount of epochs that the autoencoder is trained, it is set to only 10 for this tutorial
    parameters['batch_size'] = 256
    parameters['x'] = 200#This parameter defines the number of pixels sampled from AHN2_5m in the x direction (horizontal). This sample is used to train the autoencoder and to generate features. The sample dimensions must match the input of the autoencoder, so if different dimensions are desired, a new autoencoder must be trained.
    parameters['y'] = 200#This parameter defines the number of pixels sampled from AHN2_5m in the y direction (vertical). This sample is used to train the autoencoder and to generate features. The sample dimensions must match the input of the autoencoder, so if different dimensions are desired, a new autoencoder must be trained.
    parameters['data_set'] = 'AHN2_5m_random'#Method to get the data. This calls the "auto_encoder/load_random_tif.py" method. It is also possible to load MNIST, using 'MNIST'.
    parameters['normalize_data'] = True#This sets whether or not the input data is cast between 0.0 and 1.0. Since an autoencoder can usually only output between 0.0 and 1.0, it is highly recommended to set 'normalize_data' to True.
    parameters['train_part'] = 50#number of entries in train set. For the purpose of the tutorial this is set to quite low. Recommended are values of 1000 to 20000. This requires a lot of RAM though. I advise implementing that every batch is loaded from disk seperately.
    parameters['test_part'] = 10#number of entries in test set
    parameters['val_part'] = 10#number of entries in validation set
    parameters['n_data'] = parameters['train_part']+parameters['test_part']+parameters['val_part']#Number of entries in the test, train and validation set combined.
    parameters['print_results'] = False#Whether or not to print the results. Pretty sure this does nothing anymore.
    parameters['seed'] = 42#Seed for pseudorandom processes. Of course 42 was chosen. We apologize for the inconvenience.
    parameters['clamp'] = True#Whether of not to clamp the data between two values. Since the AHN2_5m data has extreme outliers, it is highly recommended to set this to True.
    parameters['uc'] = 210 #upper clamp value. Set in such a way that the outliers are clamped, but the usual data is not.
    parameters['lc'] = -15 #lower clamp value
    parameters['optimizer'] = 'adadelta'
    parameters['loss'] = 'binary_crossentropy'
    parameters['filter'] = 'sobel_components'#Whether or not to applly the sobel filter during preprocessing. To know what the sobel filter does, please check: https://en.wikipedia.org/wiki/Sobel_operator
    parameters['downsampling'] = False #This is not implemented
    parameters['downsample_factor'] = 4 #This is not implemented
    parameters['data_folder'] = '../AHN2_5m/'#Path to the AHN2_5m folder containing the .tif files
    parameters['results_folder'] = '../results/' + parameters['experiment_name'] + '/'#path to where the results of the experiment are stored.
    
    parameters['pooling_factor_vars'] = [1,2,5,10]#parameters specific to this experiment. Four autoencoders will be build and trained, containing an inner-most pooling layer with a factor of 1, 2, 5 and 10.
        
    skip_train_autoencoder = True
    if not skip_train_autoencoder:
        import experiments.class_experiment as class_experiment
        #Now that the parameters are set, it is time to make an instance of an experiment:
        experiment = class_experiment.Experiment(parameters)
        #To run the experiment:
        experiment.request_run()
        #After running the experiment, the saved results can be loaded and displayed by:
        experiment.load_exp()
        #It shows a plot of the loss function of the four autoencoders during training, followed by how well the autoencoders managed to reproduce the input (top two rows) in the output (bottom two rows). Plots are preformed by "experiments/plotters.py".

    #---------------------------------------------------------------------------
    #Pandafy the data
    #---------------------------------------------------------------------------

    #Now that we have all the raw data, it is time to put it into neat pandas
    #dataframes so that it can be easily combined. The pandas dataframes are
    #stored as .csv files in the folder "../pandafied_data", which should be in 
    #the superfolder of this repository. Again, don't save the pandas dataframes
    #in this repository because of their size and the private information they
    #possess.
    #The script "pandafy_data/pandafy_all.py" builds all pandas dataframes in the
    #required order. In case of retraining the autoencoder, only the script
    #"add_image_to_h5_radar.add_image_to_h5_radar()" need to be reran.
    skip_pandafy = False
    if not skip_pandafy:
        import pandafy_data.pandafy_all as pandafy_all
        encoder_file_name = 'results/tutorial_experiment/pool_1_encoder'#encoder trained in previous step
        history_file = 'results/tutorial_experiment/pool_1_history.json'#history of encoder in previous step
        pandafy_all.pandafy_all(super_folder='../',encoder_file_name=encoder_file_name,history_file=history_file,parameters=parameters)
    
    #---------------------------------------------------------------------------
    #Analyze data / build model
    #---------------------------------------------------------------------------
    #Now that all data is in an easy-to-handle format it can be analyzed. The
    #file "/pandafy_data/analyse_pandafied.py" selects and recombines the
    #data and trains a random forest on it.
    skip_analyze = False
    if not skip_analyze:
        import pandafy_data.analyse_pandafied as analyse_pandafied
        analyse_pandafied.analyse_twitter(folder='../pandafied_data/')


if __name__ == '__main__':
    start_here()

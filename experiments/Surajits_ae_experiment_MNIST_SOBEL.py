import sys
import class_experiment

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usuage: python3 experiment_script.py <run/load> (<trit/grace>)")
        exit()
    if sys.argv[1] == 'run':
        run = True
    elif sys.argv[1] == 'load':
        run = False
    else:
        print("usuage: python3 experiment_script.py <run/load> (<trit/grace>)")
        exit()
    trit = len(sys.argv) >= 3 and sys.argv[2] == 'trit'
    grace = len(sys.argv) >= 3 and sys.argv[2] == 'grace'
    print("trit: " + str(trit))
    print("grace: " + str(grace))
    
    #parameters needed for all experiments
    parameters = {}
    parameters['experiment_name'] = 'Surajits_ae_experiment_MNIST_SOBEL'
    parameters['experiment_type'] = 'varying_pooling_factor'
    parameters['encoder_type'] = 'surajit'
    #parameters['encoding_dim'] = 400 #encoding dimensions are varied for this experiment
    parameters['epochs'] = 1000
    parameters['batch_size'] = 256
    parameters['x'] = 28
    parameters['y'] = 28
    parameters['data_set'] = 'MNIST'
    parameters['normalize_data'] = True
    if run: 
        parameters['train_part'] = 10000
        parameters['test_part'] = 2000
        parameters['val_part'] = 2000
    else:
        parameters['train_part'] = 500
        parameters['test_part'] = 100
        parameters['val_part'] = 100
    parameters['n_data'] = parameters['train_part']+parameters['test_part']+parameters['val_part']
    parameters['print_results'] = False
    parameters['seed'] = 42
    #parameters['file_name'] = 'test_' #each iteration gets its own file name for this experiment
    parameters['clamp'] = True
    parameters['uc'] = 210 #upper clamp value
    parameters['lc'] = -15 #lower clamp value
    parameters['optimizer'] = 'adam'
    parameters['loss'] = 'mse'
    parameters['filter'] = 'sobel'
    parameters['downsampling'] = False #TODO do downsampling at data loading stage or in case of MNIST right after loading MNIST
    parameters['downsample_factor'] = 4
    
    if trit:
        parameters['data_folder'] = '../../../../data/s0315435/AHN2_5m/'
        parameters['results_folder'] = '../../results/' + parameters['experiment_name'] + '/'#'../../../../data/s0315435/results/' + parameters['experiment_name'] + '/'
    elif grace:
        parameters['data_folder'] = '/scratch/lamers/AHN2_5m/'
        parameters['results_folder'] = '/scratch/lamers/results/' + parameters['experiment_name'] + '/'
    else:
        parameters['data_folder'] = '../../AHN2_5m/'
        parameters['results_folder'] = '../../results/' + parameters['experiment_name'] + '/'
    
    #parameters specific to this experiment
    parameters['dims'] = [20,40,80,160,320,400]
    parameters['pooling_factor_vars'] = [1]
    
    experiment = class_experiment.Experiment(parameters)
    
    if run:
        experiment.request_run()
    else:
        experiment.load_exp()
    


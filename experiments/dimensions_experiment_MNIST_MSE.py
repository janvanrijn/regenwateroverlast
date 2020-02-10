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
    parameters['experiment_name'] = 'dimensions_experiment_MNIST_MSE'
    parameters['experiment_type'] = 'varying_encoding_dimensions'
    parameters['encoder_type'] = 'simple'
    #parameters['encoding_dim'] = 400 #encoding dimensions are varied for this experiment
    parameters['epochs'] = 50
    parameters['batch_size'] = 256
    parameters['x'] = 28
    parameters['y'] = 28
    parameters['data_set'] = 'MNIST'
    parameters['normalize_data'] = True
    if run: 
        parameters['train_part'] = 500
        parameters['test_part'] = 100
        parameters['val_part'] = 100
    else:
        parameters['train_part'] = 500
        parameters['test_part'] = 100
        parameters['val_part'] = 100
    parameters['n_data'] = parameters['train_part']+parameters['test_part']+parameters['val_part']
    parameters['print_results'] = False
    parameters['seed'] = 42
    #parameters['file_name'] = 'test_' #each iteration gets its own file name for this experiment
    parameters['clamp'] = False
    parameters['uc'] = 3.0 #upper clamp value
    parameters['lc'] = -3.0 #lower clamp value
    parameters['optimizer'] = 'sgd'
    parameters['loss'] = 'mean_squared_error'
    parameters['filter'] = 'identity'
    parameters['downsampling'] = False #TODO do downsampling at data loading stage or in case of MNIST right after loading MNIST
    parameters['downsample_factor'] = 4

    if trit:
        parameters['data_folder'] = '../../../../data/s0315435/AHN2_5m/'
        parameters['results_folder'] = '../../../../data/s0315435/results/' + parameters['experiment_name'] + '/'
    else:
        parameters['data_folder'] = '../../AHN2_5m/'
        parameters['results_folder'] = '../../results/' + parameters['experiment_name'] + '/'
    
    #parameters specific to this experiment
    parameters['dims'] = [28*28, 28*28//2, 28*28//4,28*28//8,28*28//16,28*28//32,28*28//64,28*28//128]
    
    experiment = class_experiment.Experiment(parameters)
    
    if run:
        experiment.request_run()
    else:
        experiment.load_exp()
    


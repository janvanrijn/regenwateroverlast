from snorkel.labeling import labeling_function, LabelModel, PandasLFApplier
import re
import pandas as pd
from textblob import TextBlob

ABSTAIN = -1
NOT_WATER = 0
WATER = 1

@labeling_function()
def lf_keyword_keywords(x):
    output = ABSTAIN
    if "wateroverlast" in x.text.lower():
        output = WATER
    elif "regen" in x.text.lower():
        if "m/s" in x.text.lower() or "km/h" in x.text.lower() or "luchtdr" in x.text.lower():
            output = NOT_WATER
        else:
            output = WATER
    elif "water" in x.text.lower():
        if "te water" in x.text.lower() or "voertuig" in x.text.lower() or "wagen" in x.text.lower()or "aanrijding" in x.text.lower() or "drink" in x.text.lower() or "persoon" in x.text.lower() or "brand" in x.text.lower():
            output = NOT_WATER
        else:
            output = WATER
    else:
        output = ABSTAIN
    return output

@labeling_function()
def lf_keyword_wateroverlast(x):
    return WATER if "wateroverlast" in x.text.lower() and not "bijeenkomst" in x.text.lower() else ABSTAIN

#@labeling_function()
#def lf_keyword_regen(x):
#    return WATER if "regen" in x.text.lower() else ABSTAIN

#@labeling_function()
#def lf_keyword_water(x):
#    return WATER if "water" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_voertuig(x):
    return NOT_WATER if "voertuig" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_aanrijding(x):
    return NOT_WATER if "aanrijding" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_te_water(x):
    return NOT_WATER if "te water" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_drink(x):
    return NOT_WATER if "drink" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_persoon(x):
    return NOT_WATER if "persoon" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_brand(x):
    return NOT_WATER if "brand" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_mps(x):
    return NOT_WATER if "m/s" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_kps(x):
    return NOT_WATER if "km/h" in x.text.lower() else ABSTAIN

@labeling_function()
def lf_keyword_luchtdr(x):
    return NOT_WATER if "luchtdr" in x.text.lower() else ABSTAIN

#@labeling_function()
#def lf_regex_check_out(x):
#    """Spam comments say 'check out my video', 'check it out', etc."""
#    return SPAM if re.search(r"check.*out", x.text, flags=re.I) else ABSTAIN
#
#@labeling_function()
#def lf_short_comment(x):
#    """Non-spam comments are often short, such as 'cool video!'."""
#    return NOT_SPAM if len(x.text.split()) < 5 else ABSTAIN
#
#@labeling_function()
#def lf_textblob_polarity(x):
#    """
#        We use a third-party sentiment classification model, TextBlob.
#        
#        We combine this with the heuristic that non-spam comments are often positive.
#        """
#    return NOT_SPAM if TextBlob(x.text).sentiment.polarity > 0.3 else ABSTAIN

def curate_twitter(save_name='../../pandafied_data/curated_twitter.csv'):
    df_train = pd.read_csv('../../pandafied_data/pandafied_twitter.csv')
    #from utils import load_unlabeled_spam_dataset
    #df_train = load_unlabeled_spam_dataset()
    
    # Define the set of labeling functions (LFs)
    #lfs = [lf_keyword_wateroverlast,lf_keyword_voertuig,lf_keyword_aanrijding,lf_keyword_te_water,lf_keyword_persoon,lf_keyword_brand,lf_keyword_mps,lf_keyword_kps,lf_keyword_luchtdr]

    #lfs = [lf_keyword_keywords]

    lfs = [lf_keyword_wateroverlast]

    # Apply the LFs to the unlabeled training data
    applier = PandasLFApplier(lfs)
    L_train = applier.apply(df_train)

    # Train the label model and compute the training labels
    label_model = LabelModel(cardinality=2, verbose=True)
    label_model.fit(L_train, n_epochs=500, log_freq=50, seed=123)
    df_train["label"] = label_model.predict(L=L_train, tie_break_policy="abstain")
    #tie_break_policy="true-random"
    #tie_break_policy="abstain"
    counter = 0
    for i in range(len(df_train["label"])):
        if df_train["label"][i] == WATER:
            print()
            print(df_train["text"][i])
            print(df_train["label"][i])
            print()
            counter += 1

    print("num entries total: " + str(len(df_train["label"])))
    print("num entries water: " + str(counter))

    #df_train = df_train[df_train.label != ABSTAIN]

    twitter_curated = df_train[df_train.label == WATER]
    twitter_curated = twitter_curated.drop(columns='label')
    twitter_curated.to_csv(save_name,index=False)

if __name__ == '__main__':
    curate_twitter()

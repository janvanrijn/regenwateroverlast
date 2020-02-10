import argparse
import json
import logging
import os
import pandas as pd
from PIL import Image
from osgeo import gdal
import requests
import shapely.geometry
import xmltodict
import zipfile


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--json_path', type=str, default='../data/radar.json')
    parser.add_argument('--xml_path', type=str, default='../data/ahn_index.xml')
    parser.add_argument('--output_dir', type=str, default=os.path.expanduser('~/experiments/ahn'))
    args_ = parser.parse_args()
    return args_


def download_cache_ahn(output_dir, filename, url):
    filename_zip = os.path.join(output_dir, filename)
    if not os.path.exists(filename_zip):
        r = requests.get(url)

        with open(filename_zip, 'wb') as f:
            f.write(r.content)

        with zipfile.ZipFile(filename_zip, 'r') as zip_ref:
            zip_ref.extractall(output_dir)


def run(args):
    root = logging.getLogger()
    root.setLevel(logging.INFO)

    os.makedirs(args.output_dir, exist_ok=True)
    radar_squares = dict()
    with open(args.json_path, 'r') as fp:
        data = json.load(fp)
        for d in data:
            coords = (
                (d['NW'][1], d['NW'][0]),
                (d['NE'][1], d['NE'][0]),
                (d['SE'][1], d['SE'][0]),
                (d['SW'][1], d['SW'][0]),
                # (d['NW'][1], d['NW'][0])
            )
            label = (d['X'], d['Y'])
            radar_squares[coords] = label

    with open(args.xml_path, 'r') as fp:
        ahn_index = xmltodict.parse(fp.read())
        for idx, entry in enumerate(ahn_index['feed']['entry']):
            # coords_str = entry['georss:polygon'].split(' ')
            # ahn_polygon = shapely.geometry.Polygon([
            #     (float(coords_str[0]), float(coords_str[1])),
            #     (float(coords_str[2]), float(coords_str[3])),
            #     (float(coords_str[4]), float(coords_str[5])),
            #     (float(coords_str[6]), float(coords_str[7])),
            #     # (float(coords_str[0]), float(coords_str[1])),
            # ])
            ahn_id = entry['id']
            ahn_link = entry['link'][0]['@href']
            logging.info('processing %s (%d/%d)' % (ahn_id, idx+1, len(ahn_index['feed']['entry'])))
            local_path = os.path.join(args.output_dir, ahn_id)
            download_cache_ahn(args.output_dir, ahn_id, ahn_link)
            print(local_path, os.path.isfile(local_path))
            im = Image.open(local_path[:-4], 'r')

            print(im)
            im.close()


if __name__ == '__main__':
    run(parse_args())

#!/usr/bin/env python
# -*- encoding:utf-8 -*-

import os
import sys


"""
             ffmpeg      dhash
    bmp&avi-------->img-------->tar(bmp)
    png&mp4-------->img-------->tar(png)

"""



def get_file_list(fmt, d):
    'add all fmt file in d to l'
    l=[]
    if not (os.path.exists(d)):
        print "folder %s is missing" % d
        return
    for path, dirname, files in os.walk(d):
        for filename in files:
            if filename.find(fmt)!=-1:
                l.append(path+"/"+filename)
    return l


def split_video_to_images(video, name_index, target_path, target_fmt):

    index = name_index
    if not (os.path.exists(video) and os.path.isfile(video) and os.access(video, os.R_OK)):
        print"Either file is missing or is not readable"
        return
    
    cmdstr = "mkdir .tmp && ffmpeg -i %s -f image2 .tmp/%%8d%s" % (video, target_fmt)
    os.system(cmdstr)
    
    l = get_file_list(".bmp", ".tmp")
    l.extend(get_file_list(".png", ".tmp"))

    for i in l:
        cmdstr = "mv %s %s/%08d%s" % (i, target_path, index, target_fmt)
        os.system(cmdstr)
        index+=1
    os.system("rm ./.tmp -rf")

    return index

def copy_images_to(src_fmt, src_path, target_path, name_index):
    index = name_index
    l = get_file_list(src_fmt, src_path)
    for i in l:
        cmdstr = "cp %s %s/%08d%s" % (i, target_path, index, src_fmt)
        os.system(cmdstr)
        index+=1
    return index

def split_videos_to(src_fmt, src_path, target_path, name_index, target_fmt):
    
    index = name_index
    l = get_file_list(src_fmt, src_path)
    for i in l:
        index = split_video_to_images(i, index, target_path, target_fmt)

    return index

def convert_media_to_images(img_fmt, video_fmt, org_path, img_path):
    index=0
    index = copy_images_to(img_fmt, org_path, img_path, index)
    index = split_videos_to(video_fmt, org_path, img_path, index, img_fmt)
    return


def main():

    convert_media_to_images(".png", ".mp4", "./mu1_org", "./mu1_img")
    convert_media_to_images(".bmp", ".avi", "./trs_org", "./trs_img")
    return


if __name__ == "__main__":
    main()





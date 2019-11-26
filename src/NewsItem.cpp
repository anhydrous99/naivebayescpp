//
// Created by Armando Herrera on 11/21/19.
//

#include "NewsItem.h"

bool operator==(const NewsItem &itm1, const NewsItem &itm2) {
  return itm1.path == itm2.path && itm1.collection == itm2.collection && itm1.contents == itm2.contents &&
         itm1.word_count == itm2.word_count;
}

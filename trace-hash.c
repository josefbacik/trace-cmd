/*
 * Copyright (C) 2014, Steven Rostedt <srostedt@redhat.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License (not later!)
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not,  see <http://www.gnu.org/licenses>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "trace-hash.h"

int trace_hash_init(struct trace_hash *hash, int buckets)
{
	struct list_head *bucket;

	memset(hash, 0, sizeof(*hash));

	hash->buckets = calloc(sizeof(*hash->buckets), buckets);
	if (!hash->buckets)
		return -ENOMEM;
	hash->nr_buckets = buckets;

	/* If a power of two then we can shortcut */
	if (!(buckets & (buckets - 1)))
		hash->power = buckets - 1;

	trace_hash_for_each_bucket(bucket, hash)
		list_head_init(bucket);
	return 0;
}

void trace_hash_free(struct trace_hash *hash)
{
	free(hash->buckets);
}

int trace_hash_empty(struct trace_hash *hash)
{
	struct list_head *bucket;

	trace_hash_for_each_bucket(bucket, hash)
		if (!list_empty(bucket))
			return 0;
	return 1;
}

int trace_hash_add(struct trace_hash *hash, struct trace_hash_item *item)
{
	struct list_head *bucket;
	int bucket_nr = hash->power ? item->key & hash->power :
		item->key % hash->nr_buckets;

	bucket = hash->buckets + bucket_nr;
	list_add(&item->list, bucket);

	return 1;
}

struct trace_hash_item *
trace_hash_find(struct trace_hash *hash, unsigned long long key,
		trace_hash_func match, void *data)
{
	struct trace_hash_item *item;
	struct list_head *bucket;
	int bucket_nr = hash->power ? key & hash->power :
		key % hash->nr_buckets;

	bucket = hash->buckets + bucket_nr;

	trace_hash_for_each_item(item, bucket) {
		if (item->key == key) {
			if (!match)
				return item;
			if (match(item, data))
				return item;
		}
	}

	return NULL;
}

struct trace_hash_item *
trace_hash_find_reverse(struct trace_hash *hash, unsigned long long key,
			trace_hash_func match, void *data)
{
	struct trace_hash_item *item;
	struct list_head *bucket;
	int bucket_nr = hash->power ? key & hash->power :
		key % hash->nr_buckets;

	bucket = hash->buckets + bucket_nr;
	list_for_each_entry_reverse(item, bucket, list) {
		if (item->key == key) {
			if (!match)
				return item;
			if (match(item, data))
				return item;
		}
	}

	return NULL;
}

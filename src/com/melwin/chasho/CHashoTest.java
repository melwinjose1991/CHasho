package com.melwin.chasho;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/* Author : Melwin Jose
 * Mail : melwin.jose.1991@gmail.com
 * 
 * Had a requirement to write a hashing function in C that can store 
 * 5 character aplhaNumeric string as key. 
 * 
 * The following is the proof-of-concept to check if the hashing function can 
 * distribute the keys uniformly among the buckets. The hashing function based on 
 * the hashing logic used by Java.
 */
public class CHashoTest {

	private static char[] valid = { '0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
			'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
			'y', 'z' };

	private static Map<String, Integer> validHashes;

	private static final int NO_OF_BUCKETs = 64;
	private static final int VALID_CHARS = 10 + 26; // no of valid characters in
													// 'valid' char array
	private static int ID_LEN = 5;
	private static int totalGeneratedToken = 0;

	private static int[] BUCKET_COUNT;

	// Function to generate all possible character combinations
	private static void generate(int current_index, String prevBuffer) {
		if (current_index == ID_LEN) {
			totalGeneratedToken++;

			int hash = hashThis(prevBuffer);
			int bucket = getBucket(hash);
			updateBucketCount(bucket);

			// CHECKING calculated BUCKET against actual BUCKET from validHashes
			// this is a kind of testing just to make sure the hash function
			// is working properly.
			if (validHashes.containsKey(prevBuffer)) {
				if (validHashes.get(prevBuffer) == bucket) {
					System.out.println("OK:" + prevBuffer + "\tBucket:"
							+ bucket);
				} else {
					System.out.println("NOT-OK:" + prevBuffer
							+ "\tAcutal_Bucket:" + validHashes.get(prevBuffer)
							+ "\tCalculated_Bucket:" + bucket);
				}
			}
			return;
		}
		for (int i = 0; i < VALID_CHARS; i++) {
			generate(current_index + 1, prevBuffer + valid[i]);
		}
	}

	// Hash Function
	private static int hashThis(String input) {
		int hash = 0;
		int len = input.length();
		for (int i = 0; i < len; i++) {
			char ch = input.charAt(i);
			int pow = len - 1 - i;
			hash += (ch * Math.pow(31, pow));
		}
		return hash;
	}

	private static int getBucket(int hash) {
		return hash % NO_OF_BUCKETs;
	}

	private static void updateBucketCount(int bucket_index) {
		BUCKET_COUNT[bucket_index]++;
	}

	private static void initValidHashes() {
		validHashes = new HashMap<String, Integer>();

		// Kind of Unit Testing
		validHashes.put("0", 48);
		validHashes.put("a", 33);
		validHashes.put("z", 58);

		validHashes.put("00", 0);
		validHashes.put("aa", 32);
		validHashes.put("zz", 0);

		validHashes.put("000", 48);
		validHashes.put("aaa", 1);
		validHashes.put("zzz", 58);

		validHashes.put("0000", 0);
		validHashes.put("aaaa", 0);
		validHashes.put("zzzz", 0);

		validHashes.put("00000", 48);
		validHashes.put("aaaaa", 33);
		validHashes.put("zzzzz", 58);

	}

	public static void main(String[] args) throws IOException {
		BUCKET_COUNT = new int[NO_OF_BUCKETs];
		initValidHashes();

		ID_LEN = 1;
		generate(0, "");

		ID_LEN = 2;
		generate(0, "");

		ID_LEN = 3;
		generate(0, "");

		ID_LEN = 4;
		generate(0, "");

		ID_LEN = 5;
		generate(0, "");

		int totalBucketizedTokens = 0;
		for (int i = 0; i < NO_OF_BUCKETs; i++) {
			System.out.println("Bucket[" + i + "]\t" + BUCKET_COUNT[i]);
			totalBucketizedTokens += BUCKET_COUNT[i];
		}

		System.out.println("\nBucketized Tokens : " + totalBucketizedTokens);
		System.out.println("Generated Tokens : " + totalGeneratedToken);

	}
}

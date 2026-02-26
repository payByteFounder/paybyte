# PayByte Core

PayByte Core is the reference implementation of the PayByte network — a decentralized, peer-to-peer digital currency derived from the Bitcoin and Litecoin codebases.

PayByte is an experimental cryptocurrency project designed for fast settlement, transparent issuance, and open participation.

---

## What is PayByte?

PayByte is a decentralized digital currency that enables direct peer-to-peer payments without intermediaries.

It operates using a distributed consensus model secured by proof-of-work mining. Transactions are validated and blocks are produced collectively by network participants.

PayByte Core is the open-source software that powers the network.

---

## Project Status

PayByte is currently under active development.

This repository represents the canonical reference implementation of the PayByte protocol.

The software should be considered experimental until formal releases are tagged.

---

## Origins & Acknowledgements

PayByte is derived from and would not exist without the foundational work of:

- **Bitcoin Core** — https://bitcoin.org  
- **Litecoin Core** — https://litecoin.org  

Bitcoin introduced decentralized digital scarcity and the proof-of-work blockchain model.

Litecoin extended Bitcoin’s design with faster block times and additional optimizations.

PayByte builds upon these open-source foundations under the terms of the MIT License.

All credit for the original protocol design and core architecture belongs to the respective Bitcoin and Litecoin development communities.

---

## License

PayByte Core is released under the terms of the MIT License.

This project includes code originally developed for:

- Bitcoin Core  
- Litecoin Core  

See `COPYING` for full license details.

---

## Building (Linux)

```bash
./autogen.sh
./configure
make -j$(nproc)
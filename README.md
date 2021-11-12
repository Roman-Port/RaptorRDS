# RAPTOR-RDS

RaptorRDS is an addition on top of my [RaptorDSP](https://github.com/Roman-Port/RaptorDSP) library bringing support for demodulating [RDS](https://en.wikipedia.org/wiki/Radio_Data_System) (Radio Data System), a digital standard providing text and other information on analog FM radio.

## Dependencies

RaptorRDS simply requires [RaptorDSP](https://github.com/Roman-Port/RaptorDSP) and all of it's dependencies.

## Usage

Internally, RaptorRDS is split into a number of smaller components for each task of demodulating and decoding RDS. However, most people won't need these components. Most people will be looking to use one of the following two classes:

* ``rds_pipeline`` - A combination of all of the RDS components. Simply call ``configure`` with the sample rate of the MPX then feed the MPX in with ``process``. Note that buffer you give process will be overwritten.
* ``demod_bcast_fm_stereo_rds`` - A wrapper for the pipeline that extends and acts as a drop-in replacement of ``demod_bcast_fm_stereo`` providing RDS in addition to stereo processing. No additional configuration is needed.
include(../MedicycCS2.pri)

SUBDIRS = nsingle \
    electrometer \
    main \
    touch1 \
    plugins \
    shared \
    beamline \
    quartz \
    omron \
    adc \
    ior \
    signalgenerator

main.depends = plugins shared
touch1.depends = plugins shared

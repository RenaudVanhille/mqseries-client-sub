{
  "targets": [
    {
      "include_dirs": [
        "./",
        "/opt/mqm/inc/"
      ],
      "libraries": [
        "-L/opt/mqm/lib64",
        "-lmqm"
      ],
      "target_name": "mq_sub",
      "main": "./build/Release/mq_sub.node",
      "sources": [
        "main.cc",
        "Sub/CSub.cpp",
        "Promise/CPromiseData.cpp"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "cflags": [ "-fPIC", "-std=c++14", "-pedantic", "-fexceptions", "-Wall" ],
      "cflags_cc": [ "-fPIC", "-std=c++14", "-pedantic", "-fexceptions", "-Wall" ],
    },
  ],
}
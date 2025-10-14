# Communication between ArkTS and CPP

## ArkTS-to-CPP

### Sending a Message on ArkTS
On ArkTS, you can obtain the `rnInstance` instance and call `rnInstance.postMessageToCpp(name: string, payload: any)` to send a message to CPP. This method has two parameters:
- **name**: message name, which is used to distinguish different messages during broadcast.
- **payload**: message content to be sent.
```TypeScript
this.ctx.rnInstance.postMessageToCpp("SAMPLE_MESSAGE", payload);
```

To send a message from ArkTS to `Component` or `TurboModule`, use any of the following methods to send a message to CPP:

### Receiving a Message on CPP

- Method 1: Use `onMessageReceived` to receive a message.

    1. `ComponentInstance` needs to inherit `ArkTSMessageHub::Observer` to receive a message.
        ```CPP
        class xxxComponentInstance
        : public CppComponentInstance<facebook::react::xxxShadowNode>,
          public ArkTSMessageHub::Observer
        ```
    2. Construct `ArkTSMessageHub::Observer` in the constructor.
        ```CPP
        xxxComponentInstance::xxxComponentInstance(Context context)
            : CppComponentInstance(std::move(context)),
              ArkTSMessageHub::Observer(m_deps->arkTSMessageHub) {
                ···
        }
        ```
    3. Implement the `onMessageReceived` method in `ComponentInstance` to receive a message. After a message is received, determine whether to handle the message based on the message name.
        ```CPP
        void xxxViewComponentInstance::onMessageReceived(
            ArkTSMessage const& message) {
          if (message.name == "SAMPLE_MESSAGE") {
            ···
          }
        }
        ```
- Method 2: Use `handleArkTSMessage` to receive a message.

  1. Create and implement the `ArkTSMessageHandler` class in `Package`, and implement the `handleArkTSMessage` method to handle the `message`.
      ```CPP
      class SampleArkTSMessageHandler : public ArkTSMessageHandler {
        void handleArkTSMessage(const Context& ctx) override {
          ···
        };
      };
      ```
  2. Override the `createArkTSMessageHandlers` method of the `Package` object to return the `ArkTSMessageHandler` class.
      ```CPP
      std::vector<ArkTSMessageHandler::Shared> SamplePackage::createArkTSMessageHandlers() {
        return {std::make_shared<SampleArkTSMessageHandler>()};
      }
      ```

## CPP-to-ArkTS

### Using `postMessageToArkTS` to Send a Message

1. Sending a message on CPP  
CPP uses the `postMessageToArkTS` method of the `rnInstance` object to send an event to ArkTS. This method has two parameters:
    - **name**: string, which is the name of the message to be sent.
    - **payload**: folly::dynamic, which is the data to be sent.
    ```CPP
    m_deps->rnInstance.lock()->postMessageToArkTS(messageName, messagePayload);
    ```

2. Receiving a message on ArkTS  
On ArkTS, you can subscribe to the message sent from CPP by using `cppEventEmitter` in `rnInstance` and handle the message.
    ```TypeScript
    const unsubscribe = rnInstance.cppEventEmitter.subscribe("SAMPLE_MESSAGE", (value: object) => {
      ···
      unsubscribe();
    })
    ```

### Using TurboModule to Send a Message (Not Recommended)

1. Create `TurboModule` on ArkTS and implement the corresponding API.
2. CPP obtains the `TurboModule` object.
On CPP, you can obtain the `TurboModule` object by using the `getTurboModule` method of the `rnInstance` object and convert this object to `ArkTSTurboModule`.
    ```CPP
    auto TurboModule = m_deps->rnInstance.lock()->getTurboModule<xxxTurboModule>("moduleName");
    auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
    ```
3. Handle parameters and call the corresponding method on ArkTS.
    ```CPP
    folly::dynamic payload = folly::dynamic::object;
    arkTsTurboMOudle->callSync(functionName, payload);
    ```

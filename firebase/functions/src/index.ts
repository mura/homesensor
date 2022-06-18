"use strict";

import * as functions from "firebase-functions";
import {BigQuery} from "@google-cloud/bigquery";

const bigquery = new BigQuery();

export const insertBigQuery = functions.pubsub.topic("sensors")
    .onPublish(async (message, context) => {
      const json = Buffer.from(message.data, "base64").toString();
      // console.log(json);
      if (json.match(/-connected$/)) {
        return;
      }
      const deviceId: string = message.attributes.deviceId;
      const payload = JSON.parse(json);
      // console.log(`${deviceId} Payload: ${JSON.stringfy(payload)}`);
      // console.log(context);

      if (payload) {
        const row = {
          device_id: deviceId,
          co2: payload.c,
          temperature: payload.t,
          humidity: payload.h,
          pressure: payload.p,
          time: context.timestamp,
        };
        await bigquery
            .dataset("sensors")
            .table("atmosphere")
            .insert(row);
        return 1;
        // console.log('Inserted: ' + row);
      }
      return 0;
    });

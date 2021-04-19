const {BigQuery} = require('@google-cloud/bigquery');
const bigquery = new BigQuery({
  projectId: 'homesensor'
});

/**
 * Background Cloud Function to be triggered by Pub/Sub.
 * This function is exported by index.js, and executed when
 * the trigger topic receives a message.
 *
 * @param {object} message The Pub/Sub message.
 * @param {object} context The event metadata.
 */
exports.insertBigQuery = (message, context) => {
  const json = Buffer.from(message.data, 'base64').toString();
  //console.log(json);
  if (json.match(/-connected$/)) {
    return;
  }
  const deviceId = message.attributes.deviceId 
  const payload = JSON.parse(json);
  //console.log(`${deviceId} Payload: ` + payload);
  // console.log(context);
  
  if (payload) {
    const row = {
      device_id: message.attributes.deviceId,
      co2: payload.c,
      temperature: payload.t,
      humidity: payload.h,
      pressure: payload.p,
      time: context.timestamp
    };
    bigquery
      .dataset('sensors')
      .table('atmosphere')
      .insert(row);
    //console.log('Inserted: ' + row);
  }
};


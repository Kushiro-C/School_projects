package com.example.mobile_project

import android.Manifest
import android.app.Activity
import android.app.DatePickerDialog
import android.app.TimePickerDialog
import android.content.Intent
import android.content.pm.PackageManager
import android.database.Cursor
import android.graphics.ImageDecoder
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.provider.MediaStore
import android.text.format.DateFormat
import android.util.Log
import android.view.Menu
import android.view.MenuInflater
import android.view.View
import android.widget.*
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.ViewModelProvider
import com.example.mobile_project.databinding.ActivityUpdatePlantBinding
import com.squareup.picasso.Picasso
import java.io.File
import java.text.SimpleDateFormat
import java.util.*


/**
 * UpdatePlantActivity will show the data of a Plant
 * and allow the user to update its data.
 */
class UpdatePlantActivity : AppCompatActivity() , DatePickerDialog.OnDateSetListener,
TimePickerDialog.OnTimeSetListener{
    private lateinit var binding : ActivityUpdatePlantBinding
    private lateinit var model : ViewModel
    private var dialogShowing : Int = 0 // 0 = False, 1 = Update, 2 = Delete
    private var idPlant = 0 // Id of the item
    private val REQUEST_CODE = 10021

    // Edit Texts
    private lateinit var edNom : EditText
    private lateinit var edLatin : EditText
    private lateinit var edFreq : EditText
    private lateinit var edFreq2 : EditText
    private lateinit var edFreqNutri : EditText

    // Buttons
    private lateinit var bLastWatering : Button
    private lateinit var bFreqFrom : Button
    private lateinit var bFreqTo : Button
    private lateinit var bFreqNutriFrom : Button
    private lateinit var bFreqNutriTo : Button
    private lateinit var bImg : Button
    private lateinit var bUpdate : Button
    private lateinit var bDelete : Button

    // ImageView
    private lateinit var img : ImageView
    private var imgPath : String? = "@drawable/no_image"

    // Used to update the dates (From AddPlantFragment)

    // Selected for last watering
    private var myDay: Int = 0
    private var myMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var myYear: Int = 0
    private var myHour: Int = 0
    private var myMinute: Int = 0

    // Selected for season frequency
    private var seasonFromDay: Int = 0
    private var seasonFromMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var seasonFromYear: Int = 0
    private var seasonToDay = 0
    private var seasonToMonth: Int = 0  // Months are indexed at 0, but we display with +1
    private var seasonToYear: Int = 0

    // Selected for nutrient frequency
    private var nutrientFromDay: Int = 0
    private var nutrientFromMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var nutrientFromYear: Int = 0
    private var nutrientToDay: Int = 0
    private var nutrientToMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var nutrientToYear: Int = 0

    /**
     * Returns a formatted non null String from a date with time
     */
    private fun getStringFromDateTime(date : String): String {
        val calendar: Calendar = Calendar.getInstance()
        val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd HH-mm-ss", Locale.FRENCH)
        calendar.time = simpleDateFormat.parse(date)

        val year = String.format("%02d", calendar.get(Calendar.YEAR))
        val month = String.format("%02d", calendar.get(Calendar.MONTH) + 1)
        val day = String.format("%02d", calendar.get(Calendar.DAY_OF_MONTH))
        val hour = String.format("%02d", calendar.get(Calendar.HOUR_OF_DAY))
        val minute = String.format("%02d", calendar.get(Calendar.MINUTE))

        return "$day/$month/$year $hour:$minute"
    }

    /**
     * Returns a formatted non null String from a date (without time)
     */
    private fun getStringFromDate(date : String?): String {
        if (date == null)
            return ""

        val calendar: Calendar = Calendar.getInstance()
        val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd", Locale.FRENCH)
        calendar.time = simpleDateFormat.parse(date)

        val year = String.format("%02d", calendar.get(Calendar.YEAR))
        val month = String.format("%02d", calendar.get(Calendar.MONTH) + 1)
        val day = String.format("%02d", calendar.get(Calendar.DAY_OF_MONTH))

        return "$day/$month/$year"
    }

    // OnCreate for the Menu Toolbar
    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        val inflater: MenuInflater = menuInflater
        inflater.inflate(R.menu.activity_bis_menu, menu)
        menu.findItem(R.id.action_search).isVisible = false
        /* must always return true */
        return true
    }

    @RequiresApi(Build.VERSION_CODES.N)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityUpdatePlantBinding.inflate(layoutInflater)
        setContentView(binding.root)
        setSupportActionBar(binding.myToolbar)

        /* Back button in menu toolbar */
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        model = ViewModelProvider(this).get(ViewModel::class.java)
        val context = binding.root.context

        if (savedInstanceState != null) {
            // Last Watering Date
            myDay = savedInstanceState.getInt("myDay")
            myMonth = savedInstanceState.getInt("myMonth")
            myYear = savedInstanceState.getInt("myYear")
            myHour = savedInstanceState.getInt("myHour")
            myMinute = savedInstanceState.getInt("myMinute")

            // Season From
            seasonFromDay = savedInstanceState.getInt("seasonFromDay")
            seasonFromMonth = savedInstanceState.getInt("seasonFromMonth")
            seasonFromYear = savedInstanceState.getInt("seasonFromYear")

            // Season To
            seasonToDay = savedInstanceState.getInt("seasonToDay")
            seasonToMonth = savedInstanceState.getInt("seasonToMonth")
            seasonToYear = savedInstanceState.getInt("seasonToYear")

            // Nutrient From
            nutrientFromDay = savedInstanceState.getInt("nutrientFromDay")
            nutrientFromMonth = savedInstanceState.getInt("nutrientFromMonth")
            nutrientFromYear = savedInstanceState.getInt("nutrientFromYear")

            // Nutrient To
            nutrientToDay = savedInstanceState.getInt("nutrientToDay")
            nutrientToMonth = savedInstanceState.getInt("nutrientToMonth")
            nutrientToYear = savedInstanceState.getInt("nutrientToYear")

            // Image path
            imgPath = savedInstanceState.getString("imgPath")

            // Restore the data to the buttons
            val strLast = "$myDay/${myMonth+1}/$myYear $myHour:$myMinute"
            val strFreqFrom = "$seasonFromDay/${seasonFromMonth+1}/$seasonFromYear"
            val strFreqTo = "$seasonToDay/${seasonToMonth+1}/$seasonToYear"
            val strNutriFrom = "$nutrientFromDay/${nutrientFromMonth+1}/$nutrientFromYear"
            val strNutriTo = "$nutrientToDay/${nutrientToMonth+1}/$nutrientToYear"

            if (myDay != 0 && myMinute != 0)
                binding.btnPick.text = strLast
            if (seasonFromDay != 0)
                binding.seasonFrom.text = strFreqFrom
            if (seasonToDay != 0)
                binding.seasonTo.text = strFreqTo
            if (nutrientFromDay != 0)
                binding.nutrientFrom.text = strNutriFrom
            if (nutrientToDay != 0)
                binding.nutrientTo.text = strNutriTo

            // Restore ImageView
            img = binding.addDisplayedImage
            val f = File(imgPath)
            if (f.exists()) {
                Picasso.with(img.context)
                    .load(f)
                    .into(img)
            }
        }

        loadXMLvar()

        // Buttons Actions

        //call datePicker with current date by default for normal frequency
        binding.btnPick.setOnClickListener {
            val calendar: Calendar = Calendar.getInstance()
            val day = calendar.get(Calendar.DAY_OF_MONTH)
            val month = calendar.get(Calendar.MONTH)
            val year = calendar.get(Calendar.YEAR)
            val datePickerDialog =
                DatePickerDialog(context, this, year, month, day)
            datePickerDialog.show()
        }

        //create and show datepickerdialog for season and nutrient
        fun createDatePickerDialog(): DatePickerDialog {
            val calendar: Calendar = Calendar.getInstance()
            val day = calendar.get(Calendar.DAY_OF_MONTH)
            val month = calendar.get(Calendar.MONTH)
            val year = calendar.get(Calendar.YEAR)
            return DatePickerDialog(context, this, year, month, day)
        }

        binding.seasonFrom.setOnClickListener{
            val datePickerDialog = createDatePickerDialog()
            datePickerDialog.setOnDateSetListener { _, i, i2, i3 ->
                seasonFromYear = i
                seasonFromMonth = i2
                seasonFromDay = i3
                (seasonFromDay.toString() + "/" +
                        (seasonFromMonth + 1).toString() + "/" +
                        seasonFromYear.toString()).also { binding.seasonFrom.text = it }
            }
            datePickerDialog.show()
        }

        binding.seasonTo.setOnClickListener{
            val datePickerDialog = createDatePickerDialog()
            datePickerDialog.setOnDateSetListener { _, i, i2, i3 ->
                seasonToYear = i
                seasonToMonth = i2
                seasonToDay = i3
                (seasonToDay.toString() + "/"  +
                        (seasonToMonth + 1).toString() + "/" +
                        seasonToYear.toString()).also { binding.seasonTo.text = it }
            }
            datePickerDialog.show()
        }

        binding.nutrientFrom.setOnClickListener{
            val datePickerDialog = createDatePickerDialog()
            datePickerDialog.setOnDateSetListener { _, i, i2, i3 ->
                nutrientFromYear = i
                nutrientFromMonth = i2
                nutrientFromDay = i3
                (nutrientFromDay.toString() + "/"  +
                        (nutrientFromMonth + 1).toString() + "/" +
                        nutrientFromYear.toString()).also { binding.nutrientFrom.text = it }
            }
            datePickerDialog.show()
        }

        binding.nutrientTo.setOnClickListener{
            val datePickerDialog = createDatePickerDialog()
            datePickerDialog.setOnDateSetListener { _, i, i2, i3 ->
                nutrientToYear = i
                nutrientToMonth = i2
                nutrientToDay = i3
                (nutrientToDay.toString() + "/"  +
                        (nutrientToMonth + 1).toString() + "/" +
                        nutrientToYear.toString()).also { binding.nutrientTo.text = it }
            }
            datePickerDialog.show()
        }

        binding.addImage.setOnClickListener {
            openGalleryForImage()
        }
    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)
        outState.putInt("dialogShowing", dialogShowing)

        // Last Watering Date
        outState.putInt("myDay", myDay)
        outState.putInt("myMonth", myMonth)
        outState.putInt("myYear", myYear)
        outState.putInt("myHour", myHour)
        outState.putInt("myMinute", myMinute)

        // Season From
        outState.putInt("seasonFromDay", seasonFromDay)
        outState.putInt("seasonFromMonth", seasonFromMonth)
        outState.putInt("seasonFromYear", seasonFromYear)

        // Season To
        outState.putInt("seasonToDay", seasonToDay)
        outState.putInt("seasonToMonth", seasonToMonth)
        outState.putInt("seasonToYear", seasonToYear)

        // Nutrient From
        outState.putInt("nutrientFromDay", nutrientFromDay)
        outState.putInt("nutrientFromMonth", nutrientFromMonth)
        outState.putInt("nutrientFromYear", nutrientFromYear)

        // Nutrient To
        outState.putInt("nutrientToDay", nutrientToDay)
        outState.putInt("nutrientToMonth", nutrientToMonth)
        outState.putInt("nutrientToYear", nutrientToYear)

        // Image path
        outState.putString("imgPath", imgPath)
    }

    override fun onRestoreInstanceState(savedInstanceState: Bundle) {
        super.onRestoreInstanceState(savedInstanceState)
        dialogShowing = savedInstanceState.getInt("dialogShowing")
    }

    override fun onResume() {
        super.onResume()
        // Re-show dialog after screen rotation
        if (dialogShowing == 1) // Update Dialog
            bUpdate.performClick()
        else if (dialogShowing == 2) // Delete Dialog
            bDelete.performClick()
    }

    /**
     * Loads the dates in global variables
     */
    private fun loadDates(last : String, freqFrom : String?, freqTo : String?,
                          nutriFrom : String?, nutriTo : String?) {

        val calendar: Calendar = Calendar.getInstance()
        val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd HH-mm-ss", Locale.FRENCH)
        calendar.time = simpleDateFormat.parse(last)

        myDay = calendar.get(Calendar.DAY_OF_MONTH)
        myMonth = calendar.get(Calendar.MONTH)
        myYear = calendar.get(Calendar.YEAR)
        myHour = calendar.get(Calendar.HOUR_OF_DAY)
        myMinute = calendar.get(Calendar.MINUTE)

        if (freqFrom != null) {
            val calSeasonFrom : Calendar = Calendar.getInstance()
            calSeasonFrom.time = simpleDateFormat.parse(freqFrom)
            seasonFromDay = calSeasonFrom.get(Calendar.DAY_OF_MONTH)
            seasonFromMonth = calSeasonFrom.get(Calendar.MONTH)
            seasonFromYear = calSeasonFrom.get(Calendar.YEAR)
        }

        if (freqTo != null) {
            val calSeasonTo : Calendar = Calendar.getInstance()
            calSeasonTo.time = simpleDateFormat.parse(freqTo)
            seasonToDay = calSeasonTo.get(Calendar.DAY_OF_MONTH)
            seasonToMonth = calSeasonTo.get(Calendar.MONTH)
            seasonToYear = calSeasonTo.get(Calendar.YEAR)
        }

        if (nutriFrom != null) {
            val calNutriFrom : Calendar = Calendar.getInstance()
            calNutriFrom.time = simpleDateFormat.parse(nutriFrom)
            nutrientFromDay = calNutriFrom.get(Calendar.DAY_OF_MONTH)
            nutrientFromMonth = calNutriFrom.get(Calendar.MONTH)
            nutrientFromYear = calNutriFrom.get(Calendar.YEAR)
        }

        if (nutriTo != null) {
            val calNutriTo : Calendar = Calendar.getInstance()
            calNutriTo.time = simpleDateFormat.parse(nutriTo)
            nutrientToDay = calNutriTo.get(Calendar.DAY_OF_MONTH)
            nutrientToMonth = calNutriTo.get(Calendar.MONTH)
            nutrientToYear = calNutriTo.get(Calendar.YEAR)
        }
    }

    // Load the data of the item
    private fun loadXMLvar() {
        idPlant = intent.getIntExtra("idPlant", 0)
        bImg = binding.addImage
        bUpdate = binding.bUpdate
        bDelete = binding.bDelete

        // Edit Texts
        edNom = binding.edNom
        edNom.setText(intent.getStringExtra("nom"))
        edLatin = binding.edLatin
        edLatin.setText(intent.getStringExtra("latin"))
        edFreq = binding.edFrenquence
        val f1 = intent.getIntExtra("freq", -1)
        edFreq.setText(if (f1 == -1) "" else f1.toString())
        edFreq2 = binding.edFrenquence2
        val f2 = intent.getIntExtra("freq2", -1)
        edFreq2.setText(if (f2 == -1) "" else f2.toString())
        edFreqNutri = binding.edFrenquence3
        val fNutri = intent.getIntExtra("freqNutri", -1)
        edFreqNutri.setText(if (fNutri == -1) "" else fNutri.toString())

        // On rotate we don't set old data
        if (myDay == 0 && myMinute == 0) {
            imgPath = intent.getStringExtra("imgPath")

            // Buttons
            bLastWatering = binding.btnPick
            val last = intent.getStringExtra("bLastWatering")
            bLastWatering.text = getStringFromDateTime(last)
            bFreqFrom = binding.seasonFrom
            val freqFrom = intent.getStringExtra("bFreqFrom")
            if (freqFrom != null)
                bFreqFrom.text = getStringFromDate(freqFrom)
            bFreqTo = binding.seasonTo
            val freqTo = intent.getStringExtra("bFreqTo")
            if (freqTo != null)
                bFreqTo.text = getStringFromDate(freqTo)
            bFreqNutriFrom = binding.nutrientFrom
            val nutriFrom = intent.getStringExtra("bFreqNutriFrom")
            if (nutriFrom != null)
                bFreqNutriFrom.text = getStringFromDate(nutriFrom)
            bFreqNutriTo = binding.nutrientTo
            val nutriTo = intent.getStringExtra("bFreqNutriTo")
            if (nutriTo != null)
                bFreqNutriTo.text = getStringFromDate(nutriTo)

            // Update Dates
            loadDates(last, freqFrom, freqTo, nutriFrom, nutriTo)

            // ImageView
            img = binding.addDisplayedImage
            val f = File(imgPath)
            if (f.exists()) {
                Log.d("img", "img exists Update")
                Picasso.with(img.context)
                    .load(f)
                    .into(img)
            }
        }
    }

    /**
     * Checks if the data can be inserted (From AddPlantFragment)
     */
    private fun verificationData() : Boolean {
        if(binding.edNom.text.toString().trim().isEmpty() &&
            binding.edLatin.text.toString().trim().isEmpty()
        ) {
            Toast.makeText(this, R.string.add_need_name, Toast.LENGTH_SHORT).show()
            return false
        }
        if(binding.edFrenquence.text.toString() == ""
            || binding.edFrenquence.text.toString().toInt() <= 0) {
            Toast.makeText(this, R.string.add_need_freq, Toast.LENGTH_SHORT).show()
            return false
        }
        if(myDay == 0 && myMinute == 0) {
            Toast.makeText(this, R.string.add_need_last, Toast.LENGTH_SHORT).show()
            return false
        }
        if(binding.edFrenquence2.text.toString() != "") {
            if(seasonFromDay == 0 || seasonToDay == 0) {
                Toast.makeText(this, R.string.add_need_season, Toast.LENGTH_SHORT).show()
                return false
            }
        }
        if(binding.edFrenquence3.text.toString() != "") {
            if(nutrientFromDay == 0 || nutrientToDay == 0) {
                Toast.makeText(this, R.string.add_need_nutrient, Toast.LENGTH_SHORT).show()
                return false
            }
        }
        return true
    }

    // Update button onClick action
    fun updateAction(view : View) {
        dialogShowing = 1 // Show this dialog after rotation

        val builder = AlertDialog.Builder(this@UpdatePlantActivity)
        builder.setMessage(R.string.confirmUpdate)
            .setCancelable(false)
            .setPositiveButton(R.string.yes) { dialog, _ ->
                // Update the database

                // Incoherent data, can not update
                if (!verificationData()) {
                    dialog.dismiss()
                    dialogShowing = 0
                    return@setPositiveButton
                }

                // FROM AddPlantFragment
                val currentCalendar: Calendar = Calendar.getInstance()

                //normal frequency
                val calendar: Calendar = Calendar.getInstance()
                calendar.set(myYear, myMonth, myDay, myHour, myMinute)
                val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd HH-mm-ss", Locale.FRENCH)
                val dateTime = simpleDateFormat.format(calendar.time)
                if(currentCalendar < calendar) {
                    Toast.makeText(this, R.string.add_err_date, Toast.LENGTH_SHORT).show()
                    dialog.dismiss()
                    dialogShowing = 0
                    return@setPositiveButton
                }

                //season frequency and dates
                val freq2: Int?
                val dateSeasonFrom: String?
                val calendarSeasonFrom: Calendar = Calendar.getInstance()
                val dateSeasonTo: String?
                val calendarSeasonTo: Calendar = Calendar.getInstance()
                if(binding.edFrenquence2.text.toString() == "") {
                    freq2 = null
                    dateSeasonFrom = null
                    dateSeasonTo = null
                }
                else {
                    freq2 = binding.edFrenquence2.text.toString().toInt()
                    calendarSeasonFrom.set(seasonFromYear, seasonFromMonth, seasonFromDay)
                    dateSeasonFrom = simpleDateFormat.format(calendarSeasonFrom.time)
                    calendarSeasonTo.set(seasonToYear, seasonToMonth, seasonToDay)
                    dateSeasonTo = simpleDateFormat.format(calendarSeasonTo.time)

                }
                if(calendarSeasonFrom > calendarSeasonTo) {
                    Toast.makeText(this, R.string.add_err_date, Toast.LENGTH_SHORT).show()
                    return@setPositiveButton
                }

                //nutrient frequency and dates
                val freq3: Int?
                val dateNutrientFrom: String?
                val calendarNutrientFrom: Calendar = Calendar.getInstance()
                val dateNutrientTo: String?
                val calendarNutrientTo: Calendar = Calendar.getInstance()
                if (binding.edFrenquence3.text.toString() == "") {
                    freq3 = null
                    dateNutrientFrom = null
                    dateNutrientTo = null
                }
                else {
                    freq3 = binding.edFrenquence3.text.toString().toInt()
                    calendarNutrientFrom.set(nutrientFromYear, nutrientFromMonth, nutrientFromDay)
                    dateNutrientFrom = simpleDateFormat.format(calendarNutrientFrom.time)
                    calendarNutrientTo.set(nutrientToYear, nutrientToMonth, nutrientToDay)
                    dateNutrientTo = simpleDateFormat.format(calendarNutrientTo.time)
                }
                if(calendarNutrientFrom > calendarNutrientTo) {
                    Toast.makeText(this, R.string.add_err_date, Toast.LENGTH_SHORT).show()
                    return@setPositiveButton
                }

                // New data
                val nom =
                    if (binding.edNom.text.toString().trim().isNotBlank())
                        binding.edNom.text.toString().trim()
                    else
                        null
                val latin =
                    if (binding.edLatin.text.toString().trim().isNotBlank())
                        binding.edLatin.text.toString().trim()
                    else
                        null

                // Updating the plant
                val myPlant = Plant(idPlant, nom, latin,
                    binding.edFrenquence.text.toString().toInt(),
                    freq2, dateSeasonFrom, dateSeasonTo,
                    freq3, dateNutrientFrom, dateNutrientTo, dateTime, imgPath)

                model.updateData(myPlant)

                // Sending Update OK message to parent
                val intUpdOK = Intent()
                intUpdOK.putExtra("actionOK", "update")
                setResult(RESULT_OK, intUpdOK)
                finish()
            }
            .setNegativeButton(R.string.no) { dialog, _ ->
                dialog.dismiss()
                dialogShowing = 0
            }

        val alert = builder.create()
        alert.show()
    }

    // Delete button onClick action
    fun deleteAction(view : View) {
        dialogShowing = 2 // Show this dialog after rotation

        val builder = AlertDialog.Builder(this@UpdatePlantActivity)
        builder.setMessage(R.string.confirmDelete)
            .setCancelable(false)
            .setPositiveButton(R.string.yes) { _, _ ->
                // Delete from database
                model.deleteData(idPlant)

                // Sending Delete OK message to parent
                val intDelOK = Intent()
                intDelOK.putExtra("actionOK", "delete")
                setResult(RESULT_OK, intDelOK)
                finish()
            }
            .setNegativeButton(R.string.no) { dialog, _ ->
                dialog.dismiss()
                dialogShowing = 0
            }

        val alert = builder.create()
        alert.show()
    }

    // Aux Functions to access Gallery and modify dates (Adapted from AddPlantFragment)

    //onRequestPermissionsResult
    private val requestPermissionLauncher =
        registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { isGranted: Boolean ->
            if (isGranted) {
                Toast.makeText(this, "Permission Granted", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(this, "Permission Denied", Toast.LENGTH_SHORT).show()
            }
        }

    @RequiresApi(Build.VERSION_CODES.M)
    private fun openGalleryForImage() {

        when {
            this.let {
                ContextCompat.checkSelfPermission(
                    it,
                    Manifest.permission.READ_EXTERNAL_STORAGE
                )
            } == PackageManager.PERMISSION_GRANTED -> {
                // You can use the API that requires the permission.
                val intent = Intent(Intent.ACTION_PICK)
                intent.type = "image/*"
                startActivityForResult(intent, REQUEST_CODE)
            }
            shouldShowRequestPermissionRationale(Manifest.permission.READ_EXTERNAL_STORAGE) -> {
                this@UpdatePlantActivity.let {
                    ActivityCompat.requestPermissions(
                        it,
                        arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE), 1)
                }
            }
            else -> { //directly ask for the permission.
                requestPermissionLauncher.launch(
                    Manifest.permission.READ_EXTERNAL_STORAGE)
            }
        }
    }

    private fun getRealPathFromURI(contentURI: Uri): String? {
        val result: String
        val cursor: Cursor? = this@UpdatePlantActivity.applicationContext?.contentResolver?.query(contentURI, null, null, null, null)
        if (cursor == null) { // Source is Dropbox or other similar local file path
            result = contentURI.path.toString()
        } else {
            cursor.moveToFirst()
            val idx: Int = cursor.getColumnIndex(MediaStore.Images.ImageColumns.DATA)
            result = cursor.getString(idx)
            cursor.close()
        }
        return result
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode == Activity.RESULT_OK && requestCode == REQUEST_CODE){
            Toast.makeText(this@UpdatePlantActivity, data?.data.toString(), Toast.LENGTH_SHORT).show()
            //img = data?.data.toString()
            imgPath = data?.data?.let { getRealPathFromURI(it).toString() }
            Log.d("img", imgPath)
            val bitmap = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                data?.data?.let {
                    ImageDecoder.createSource(this.contentResolver,
                        it
                    )
                }?.let { ImageDecoder.decodeBitmap(it) }
            } else {
                MediaStore.Images.Media.getBitmap(this.contentResolver, data?.data) //TOFIX: warning
            }

            binding.addDisplayedImage.setImageBitmap(bitmap)
        }
    }

    /**
     * update myDay, MyYear, myMonth and then call TimePicker only for last frequency
     */
    override fun onDateSet(view: DatePicker?, year: Int, month: Int, dayOfMonth: Int) {
        myDay = dayOfMonth
        myYear = year
        myMonth = month
        val calendar: Calendar = Calendar.getInstance()
        myHour = calendar.get(Calendar.HOUR)
        myMinute = calendar.get(Calendar.MINUTE)
        val timePickerDialog = TimePickerDialog(
            this@UpdatePlantActivity,  this, myHour, myMinute,
            DateFormat.is24HourFormat(this@UpdatePlantActivity))
        timePickerDialog.show()
    }

    /**
     * display date and time choices onTimeSet only for last frequency
     */
    override fun onTimeSet(view: TimePicker?, hourOfDay: Int, minute: Int) {
        myHour = hourOfDay
        myMinute = minute
        val date = "$myDay/${myMonth+1}/$myYear $myHour:$myMinute"
        binding.btnPick.text = date
    }
}
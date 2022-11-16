package com.example.mobile_project

import android.Manifest
import android.app.Activity
import android.app.AlertDialog
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.example.mobile_project.databinding.FragmentAddPlantBinding

import android.app.DatePickerDialog
import android.app.TimePickerDialog
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.database.Cursor
import android.graphics.ImageDecoder
import android.net.Uri
import android.os.Build
import android.text.format.DateFormat
import androidx.annotation.RequiresApi
import java.text.SimpleDateFormat
import java.util.*
import android.provider.MediaStore
import android.util.Log
import android.widget.*
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.fragment.app.activityViewModels
import com.squareup.picasso.Picasso
import java.io.File


/**
 * Fragment class to Add a Plant
 */
class AddPlantFragment : Fragment(R.layout.fragment_add_plant), DatePickerDialog.OnDateSetListener,
    TimePickerDialog.OnTimeSetListener {
    private val model : ViewModel by activityViewModels()
    private lateinit var lastPlant : Plant // Last insertion we tried, use to update
    private lateinit var dialog : AlertDialog
    private lateinit var binding : FragmentAddPlantBinding
    private val REQUEST_CODE = 10021

    // image
    private var img: String? = "@drawable/no_image"

    // selected for last watering
    private var myDay = 0
    private var myMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var myYear: Int = 0
    private var myHour: Int = 0
    private var myMinute: Int = 0

    //selected for season frequency
    private var seasonFromDay = 0
    private var seasonFromMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var seasonFromYear: Int = 0
    private var seasonToDay = 0
    private var seasonToMonth: Int = 0
    private var seasonToYear: Int = 0

    //selected for nutrient frequency
    private var nutrientFromDay = 0
    private var nutrientFromMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var nutrientFromYear: Int = 0
    private var nutrientToDay = 0
    private var nutrientToMonth: Int = 0 // Months are indexed at 0, but we display with +1
    private var nutrientToYear: Int = 0

    private val fre = "5"

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)

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
        outState.putString("imgPath", img)
    }


    @RequiresApi(Build.VERSION_CODES.N)
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding = FragmentAddPlantBinding.bind( view )


        //tap this lines into a terminal to test on emulator
        //adb shell am start -W -a android.intent.action.VIEW -d "https://www.myplant_mobile.com/test/cecile"
        // adb shell am start -a android.intent.action.VIEW -d "https://www.myplant_mobile.com/test/cecile"
        arguments?.getString("qrcode")?.let {
            if(it != "null") {
                binding.edNom.setText(it.substring("https://www.myplant_mobile.com/test/".length))
                val calendar: Calendar = Calendar.getInstance()
                myDay = calendar.get(Calendar.DAY_OF_MONTH)
                myYear = calendar.get(Calendar.YEAR)
                myMonth = calendar.get(Calendar.MONTH)
                myHour = calendar.get(Calendar.HOUR)
                myMinute = calendar.get(Calendar.MINUTE)
                val strLast = "$myDay/${myMonth+1}/$myYear $myHour:$myMinute"
                if (myDay != 0 && myMinute != 0)
                    binding.btnPick.text = strLast
                binding.edFrenquence.setText(fre)
            }
        }

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
            img = savedInstanceState?.getString("imgPath")

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

            val f = File(img)
            if (f.exists()) {
                Picasso.with(context)
                    .load(f)
                    .into(binding.addDisplayedImage)
            }

        }

        // Check if we have successfully inserted, or we need to update
        // In the latter case, we update the pays
        model.toUpdate.observe(viewLifecycleOwner) {
            if (it == -1L) {
                dialog = AlertDialog.Builder(view.context)
                    .setMessage( getString(R.string.db_failed)+ " ${lastPlant.idPlant} ?")
                    .setCancelable(false)
                    .setPositiveButton(R.string.yes) { d, _ ->
                        model.updateData(lastPlant)
                        d.dismiss()
                    }
                    .setNegativeButton(R.string.no) { d, _ -> d.dismiss() }
                    .create()

                dialog.show()
            }
        }

        //call datePicker with current date by default for normal frequency
        binding.btnPick.setOnClickListener {
            val calendar: Calendar = Calendar.getInstance()
            val day = calendar.get(Calendar.DAY_OF_MONTH)
            val month = calendar.get(Calendar.MONTH)
            val year = calendar.get(Calendar.YEAR)
            val datePickerDialog =
                DatePickerDialog(view.context, this, year, month, day)
            datePickerDialog.show()
        }

        //create and show datepickerdialog for season and nutrient
        fun createDatePickerDialog(): DatePickerDialog {
            val calendar: Calendar = Calendar.getInstance()
            val day = calendar.get(Calendar.DAY_OF_MONTH)
            val month = calendar.get(Calendar.MONTH)
            val year = calendar.get(Calendar.YEAR)
            return DatePickerDialog(view.context, this, year, month, day)
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

        /**
         * Button "ajouter" adds data in database
         */
        binding.bAdd.setOnClickListener {
            //verification
            if(binding.edNom.text.toString().trim().isEmpty() &&
                binding.edLatin.text.toString().trim().isEmpty()
            ) {
                Toast.makeText(activity, R.string.add_need_name, Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if(binding.edFrenquence.text.toString() == ""
                || binding.edFrenquence.text.toString().toInt() <= 0) {
                Toast.makeText(activity, R.string.add_need_freq, Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if(myDay == 0 && myMinute == 0) {
                Toast.makeText(activity, R.string.add_need_last, Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }
            if(binding.edFrenquence2.text.toString() != "") {
                if(seasonFromDay == 0 || seasonToDay == 0) {
                    Toast.makeText(activity, R.string.add_need_season, Toast.LENGTH_SHORT).show()
                    return@setOnClickListener
                }
            }
            if(binding.edFrenquence3.text.toString() != "") {
                if(nutrientFromDay == 0 || nutrientToDay == 0) {
                    Toast.makeText(activity, R.string.add_need_nutrient, Toast.LENGTH_SHORT).show()
                    return@setOnClickListener
                }
            }

            // add in database
            val currentCalendar: Calendar = Calendar.getInstance()

            //normal frequency
            val calendar: Calendar = Calendar.getInstance()
            calendar.set(myYear, myMonth, myDay, myHour, myMinute)
            val simpleDateFormat = SimpleDateFormat("yyyy-MM-dd HH-mm-ss", Locale.FRENCH)
            val dateTime = simpleDateFormat.format(calendar.time)
            if(currentCalendar < calendar) {
                Toast.makeText(activity, R.string.add_err_date, Toast.LENGTH_SHORT).show()
                return@setOnClickListener
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
                Toast.makeText(activity, R.string.add_err_date, Toast.LENGTH_SHORT).show()
                return@setOnClickListener
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
                Toast.makeText(activity, R.string.add_err_date, Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

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

            val myPlant = Plant(0, nom, latin, binding.edFrenquence.text.toString().toInt(),
                freq2, dateSeasonFrom, dateSeasonTo,
                freq3, dateNutrientFrom, dateNutrientTo, dateTime, img)

            model.insertData(myPlant)

            //re display
            model.loadAll()

            Toast.makeText(activity, "Data Inserted", Toast.LENGTH_SHORT).show()

        }
    }

    //onRequestPermissionsResult
    private val requestPermissionLauncher =
        registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { isGranted: Boolean ->
            if (isGranted) {
                Toast.makeText(context, "Permission Granted", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(context, "Permission Denied", Toast.LENGTH_SHORT).show()
            }
        }

    private fun openGalleryForImage() {

        when {
            context?.let {
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
                activity?.let {
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
        val cursor: Cursor? = activity?.applicationContext?.contentResolver?.query(
            contentURI, null, null, null, null)
        if (cursor == null) // Source is Dropbox or other similar local file path
            result = contentURI.path.toString()
        else {
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
            Toast.makeText(activity, data?.data.toString(), Toast.LENGTH_SHORT).show()
            img = data?.data?.let { getRealPathFromURI(it).toString() }
            Log.d("img", img!!)
            val bitmap = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                data?.data?.let {
                    ImageDecoder.createSource(requireContext().contentResolver,
                        it
                    )
                }?.let { ImageDecoder.decodeBitmap(it) }
            } else {
                MediaStore.Images.Media.getBitmap(requireContext().contentResolver, data?.data)
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
            activity,  this, myHour, myMinute,
            DateFormat.is24HourFormat(activity))
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

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_add_plant, container, false)
    }

    /*override fun onAttach(context: Context) {
        super.onAttach(context)
        arguments?.getString("qrcode")?.let {
            binding.edNom.setText(it)
        }
    }*/

    companion object {
        @JvmStatic
        fun newInstance(qrcode: String) =
            AddPlantFragment().apply {
                arguments = Bundle().apply {
                    putString("qrcode", qrcode)
                }
            }
    }
}